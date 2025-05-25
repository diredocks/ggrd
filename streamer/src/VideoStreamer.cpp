// src/VideoStreamer.cpp
#include "VideoStreamer.hpp"
#include "FaceRecognizer.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

void VideoStreamer::captureLoop() {
  cv::Mat frame;
  std::vector<uchar> buffer;

  while (running_) {
    if (videoClients_.empty()) {
      if (cap_.isOpened()) {
        cap_.release();
        spdlog::info("no active connection, released camera");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    if (!cap_.isOpened()) {
      cap_.open(0, cv::CAP_V4L2);
      if (!cap_.isOpened()) {
        spdlog::critical("unable to open cam");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      cap_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
      cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }

    cap_ >> frame;
    if (frame.empty()) {
      spdlog::warn("skpping empty frame");
      continue;
    }

    if (!cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, 40})) {
      spdlog::critical("failed to encode frame");
      continue;
    }

    for (auto ws : videoClients_) {
      ws->send(std::string_view(reinterpret_cast<char *>(buffer.data()),
                                buffer.size()),
               uWS::OpCode::BINARY);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    if (frameCount_ >= FRAME_TO_DEC) {
      // push into frame queue then notify detect thread
      std::unique_lock lk(queueMutex_);
      // drop oldest frame if queue full
      if (frameQueue_.size() >= maxQueueSize_)
        frameQueue_.pop_front();
      frameQueue_.push_back(frame);
      queueCond_.notify_one();
      frameCount_ = 0; // reset counter
    }
    frameCount_++;
  }
}

void VideoStreamer::faceDetectLoop() {
  FaceRecognizer recognizer;

  while (running_) {
    if (faceClients_.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    cv::Mat frame;

    std::unique_lock lk(queueMutex_);
    // wait for queue till it got something
    queueCond_.wait(lk, [this] { return !frameQueue_.empty() || !running_; });
    if (!running_)
      break;
    frame = frameQueue_.front();
    frameQueue_.pop_front();

    // let's detect
    recognizer.setFrame(frame);
    std::vector<FaceInfo> faces = std::move(recognizer.detect());

    // build JSON
    nlohmann::json j;
    j["faces"] = nlohmann::json::array();
    for (auto &f : faces) {
      j["faces"].push_back({{"x", f.bbox.x},
                            {"y", f.bbox.y},
                            {"w", f.bbox.width},
                            {"h", f.bbox.height},
                            {"id", f.id},
                            {"label", f.label}});
    }
    auto msg = j.dump();

    // push to clients
    for (auto *ws : faceClients_) {
      ws->send(msg, uWS::OpCode::TEXT);
    }
  }
}
