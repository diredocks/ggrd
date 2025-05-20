// src/FaceRecognizer.cpp
#include "FaceRecognizer.hpp"
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/opencv.h>
#include <filesystem>
#include <spdlog/spdlog.h>

std::vector<FaceInfo> FaceRecognizer::detect() {
  if (frame_.empty()) {
    spdlog::warn("empty frame in detect()");
    return {};
  }

  auto detectedFaces = detectFacesInFrame();
  if (!detectedFaces.empty()) {
    matchFaces(detectedFaces);
  }
  // update tracked faces
  updateTrackedFaces(detectedFaces);

  return detectedFaces;
}

std::vector<FaceInfo> FaceRecognizer::detectFacesInFrame() {
  std::vector<FaceInfo> faces;
  dlib::cv_image<dlib::bgr_pixel> cimg(frame_);
  auto detections = detector_(cimg);

  for (const auto &rect : detections) {
    FaceInfo face;
    face.bbox = cv::Rect(cv::Point(rect.left(), rect.top()),
                         cv::Point(rect.right(), rect.bottom()));
    face.rect = rect;
    faces.push_back(face);
  }

  return faces;
}

void FaceRecognizer::matchFaces(std::vector<FaceInfo> &detectedFaces) {
  for (auto &detected : detectedFaces) {
    int bestMatchIdx = -1;
    double bestDistance = DIST_THRESHOLD;

    cv::Point detectedCenter = centerOf(detected.bbox);

    for (size_t i = 0; i < trackedFaces_.size(); ++i) {
      // find smallest dist from all candidates
      const auto &candidate = trackedFaces_[i];
      cv::Point trackedCenter = centerOf(candidate.bbox);
      auto dist = euclideanDist(detectedCenter, trackedCenter);

      if (dist < bestDistance) {
        bestDistance = dist;
        bestMatchIdx = static_cast<int>(i);
      }
    }

    if (bestMatchIdx >= 0) {
      const auto &match = trackedFaces_[bestMatchIdx];
      detected.id = match.id;
      detected.label = match.label;
      detected.lostCount = 0;
    } else {
      // new face
      detected.id = nextLabelId_++;
      if (auto maybe_label = recognizeFace(detected); maybe_label.has_value()) {
        detected.label = std::to_string(detected.id) + " | " + *maybe_label;
        spdlog::info("recognized face: {}", *maybe_label);
      } else {
        detected.label = std::to_string(detected.id) + " | Unknown";
        spdlog::info("new unknown face detected, id={}", detected.id);
      }
      detected.lostCount = 0;
    }
  }
}

void FaceRecognizer::updateTrackedFaces(
    const std::vector<FaceInfo> &newDetections) {
  std::vector<FaceInfo> updated;

  for (auto &tracked : trackedFaces_) {
    bool isMatched =
        std::any_of(newDetections.begin(), newDetections.end(),
                    [&](const FaceInfo &det) { return det.id == tracked.id; });

    if (!isMatched) {
      tracked.lostCount++;
      if (tracked.lostCount <= MAX_LOST_COUNT) {
        updated.push_back(tracked);
      } else {
        // tracked will get removed here
        spdlog::info("face id={} removed after {} lost frames", tracked.id,
                     tracked.lostCount);
      }
    }
  }

  // add current frame
  updated.insert(updated.end(), newDetections.begin(), newDetections.end());
  trackedFaces_ = std::move(updated);
}

void FaceRecognizer::loadKnownFaces(const std::string &folder) {
  for (const auto &entry : std::filesystem::directory_iterator(folder)) {
    if (entry.is_regular_file()) {
      std::string label = entry.path().stem().string();
      cv::Mat img = cv::imread(entry.path().string());
      if (img.empty())
        continue;

      dlib::cv_image<dlib::bgr_pixel> cimg(img);
      std::vector<dlib::rectangle> dets = detector_(cimg); // face detector
      if (dets.empty())
        continue;

      dlib::full_object_detection shape = pose_model_(cimg, dets[0]);
      dlib::matrix<dlib::rgb_pixel> face_chip;
      dlib::extract_image_chip(
          cimg, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);

      dlib::matrix<float, 0, 1> descriptor = net_(face_chip);
      known_descriptors_.push_back(descriptor);
      known_labels_.push_back(label);
    }
  }

  spdlog::info("loaded {} known faces", known_labels_.size());
}

std::optional<std::string>
FaceRecognizer::recognizeFace(const FaceInfo &faceInfo) {
  dlib::cv_image<dlib::bgr_pixel> cimg(frame_);
  dlib::rectangle rect = faceInfo.rect;

  if (rect.is_empty())
    return std::nullopt;

  dlib::full_object_detection shape = pose_model_(cimg, rect);
  dlib::matrix<dlib::rgb_pixel> face_chip;
  dlib::extract_image_chip(cimg, dlib::get_face_chip_details(shape, 150, 0.25),
                           face_chip);
  dlib::matrix<float, 0, 1> descriptor = net_(face_chip);

  double bestDist = BEST_KNOWN_THRESHOLD;
  int bestIdx = -1;

  for (size_t i = 0; i < known_descriptors_.size(); ++i) {
    double dist = length(descriptor - known_descriptors_[i]);
    if (dist < bestDist) {
      bestDist = dist;
      bestIdx = i;
    }
  }

  if (bestIdx >= 0)
    return known_labels_[bestIdx];
  return std::nullopt;
}
