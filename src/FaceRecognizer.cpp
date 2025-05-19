// src/FaceRecognizer.cpp
#include "FaceRecognizer.h"

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/opencv.h>
#include <spdlog/spdlog.h>

/*
std::vector<FaceInfo> FaceRecognizer::detect() {
std::vector<FaceInfo> results;

if (frame_.empty()) {
  spdlog::warn("empty frame in detect()");
  return results;
}

dlib::cv_image<dlib::bgr_pixel> cimg(frame_);
std::vector<dlib::rectangle> faces = detector_(cimg); // detect faces

  for (const auto &face : faces) {
    // TODO: we should be holding here if face already detected

    dlib::full_object_detection shape = pose_model_(cimg, face); // get points
    dlib::matrix<dlib::rgb_pixel> face_chip; // chip to 150x150
    dlib::extract_image_chip(
        cimg, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);
    auto face_desc = net_(face_chip); // get description vectors


    FaceInfo info;
    info.bbox = cv::Rect(cv::Point(face.left(), face.top()),
                         cv::Point(face.right(), face.bottom()));
    info.label = "Unknown"; // 可添加识别逻辑
    results.push_back(info);
  }

  return results;
}
*/

std::vector<FaceInfo> FaceRecognizer::detect() {
  if (frame_.empty()) {
    spdlog::warn("empty frame in detect()");
    return {};
  }

  auto detectedFaces = detectFacesInFrame();
  if (detectedFaces.empty()) {
    // reset state if no face detected
    trackedFaces_.clear();
    nextLabelId_ = 0;
    return {};
  }
  // update tracked faces
  matchFaces(detectedFaces);
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
    faces.push_back(face);
  }

  return faces;
}

void FaceRecognizer::matchFaces(std::vector<FaceInfo> &detectedFaces) {
  for (auto &detected : detectedFaces) {
    int bestMatchIdx = -1;
    double bestDistance = MATCH_DIST_THRESHOLD;

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
      detected.label = std::to_string(detected.id);
      detected.lostCount = 0;
      spdlog::info("new face detected, id={0:d}", detected.id);
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
      }
    }
  }

  // add current frame
  updated.insert(updated.end(), newDetections.begin(), newDetections.end());
  trackedFaces_ = std::move(updated);
}
