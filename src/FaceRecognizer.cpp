// src/FaceRecognizer.cpp
#include "FaceRecognizer.h"

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/opencv.h>

std::vector<FaceInfo> FaceRecognizer::detect() {
  std::vector<FaceInfo> results;

  if (frame_.empty()) {
    spdlog::warn("empty frame in detect()");
    return results;
  }

  dlib::cv_image<dlib::bgr_pixel> cimg(frame_);
  std::vector<dlib::rectangle> faces = detector_(cimg);

  for (const auto &face : faces) {
    dlib::full_object_detection shape = pose_model_(cimg, face);
    dlib::matrix<dlib::rgb_pixel> face_chip;
    dlib::extract_image_chip(
        cimg, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);

    FaceInfo info;
    info.bbox = cv::Rect(cv::Point(face.left(), face.top()),
                         cv::Point(face.right(), face.bottom()));
    info.label = "Unknown"; // 可添加识别逻辑
    results.push_back(info);
  }

  return results;
}
