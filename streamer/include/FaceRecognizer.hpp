// include/FaceRecognizer.h
#pragma once

#include "ConfigManager.hpp"
#include <dlib/dnn.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

using namespace dlib;

template <template <int, template <typename> class, int, typename> class block,
          int N, template <typename> class BN, typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <template <int, template <typename> class, int, typename> class block,
          int N, template <typename> class BN, typename SUBNET>
using residual_down =
    add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block =
    BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET>
using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET>
using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET>
using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET>
using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET>
using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<
    128,
    avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<max_pool<
        3, 3, 2, 2,
        relu<affine<con<32, 7, 7, 2, 2, input_rgb_image_sized<150>>>>>>>>>>>>>;

struct FaceInfo {
  cv::Rect bbox;
  std::string label;
  dlib::rectangle rect;
  int id = -1;       // unique id
  int lostCount = 0; // lost frame counter
};

class FaceRecognizer {
public:
  FaceRecognizer() {
    try {
      dlib::deserialize(config_.face_recognition.model_path) >> pose_model_;
      dlib::deserialize(config_.face_recognition.face_recognition_model_path) >>
          net_;
      loadKnownFaces(config_.face_recognition.known_faces_folder);
    } catch (const std::exception &e) {
      spdlog::error("error loading: {}", e.what());
      std::exit(EXIT_FAILURE);
    }
  }

  void setFrame(const cv::Mat &frame) { frame_ = frame; };
  std::vector<FaceInfo> detect();

private:
  const Config &config_ = ConfigManager::getInstance().getConfig();

  cv::Mat frame_;
  dlib::frontal_face_detector detector_ = dlib::get_frontal_face_detector();

  dlib::shape_predictor pose_model_;
  anet_type net_;
  std::vector<dlib::matrix<float, 0, 1>> known_descriptors_;
  std::vector<std::string> known_labels_;
  const double BEST_KNOWN_THRESHOLD =
      config_.face_recognition.best_known_threshold; // = 0.5;
  void loadKnownFaces(const std::string &folder);
  std::optional<std::string> recognizeFace(const FaceInfo &faceInfo);

  // vector of last frame faces
  std::vector<FaceInfo> trackedFaces_;
  // id for new faces
  int nextLabelId_ = 0;
  // max lost frame count
  const int MAX_LOST_COUNT = config_.face_recognition.max_lost_count; // = 5;
  // matching distance (in pixel)
  const double DIST_THRESHOLD =
      config_.face_recognition.distance_threshold; // = 100.0;

  void updateTrackedFaces(const std::vector<FaceInfo> &newDetections);
  void matchFaces(std::vector<FaceInfo> &detectedFaces);
  std::vector<FaceInfo> detectFacesInFrame();

  // dist of two points
  double euclideanDist(const cv::Point &a, const cv::Point &b) const {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
  }
  // center of a rect
  cv::Point centerOf(const cv::Rect &rect) const {
    return cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
  }
};
