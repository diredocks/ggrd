// include/VideoStreamer.h
#pragma once

#include "ConfigManager.hpp"
#include "Types.hpp"
#include <App.h>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

class VideoStreamer {
public:
  VideoStreamer(
      std::vector<uWS::WebSocket<false, true, PerSocketData> *> &videoClients,
      std::vector<uWS::WebSocket<false, true, PerSocketData> *> &faceClients)
      : videoClients_(videoClients), faceClients_(faceClients) {};
  ~VideoStreamer() { stop(); };
  void start() {
    captureThread_ = std::thread(&VideoStreamer::captureLoop, this);
    detectThread_ = std::thread(&VideoStreamer::faceDetectLoop, this);
  };
  void stop() {
    running_ = false;
    queueCond_.notify_all(); // stop waiting plz
    if (captureThread_.joinable()) {
      captureThread_.join();
    }
    if (detectThread_.joinable()) {
      detectThread_.join();
    }
    if (cap_.isOpened()) {
      cap_.release();
    }
  };

private:
  const Config &config_ = ConfigManager::getInstance().getConfig();

  void captureLoop();    // capture and stream
  void faceDetectLoop(); // recognize and push

  // vector of clients
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> &videoClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> &faceClients_;

  // queue and sync
  std::deque<cv::Mat> frameQueue_;
  std::mutex queueMutex_;
  std::condition_variable queueCond_;
  const size_t maxQueueSize_ = 5;

  cv::VideoCapture cap_;
  std::thread captureThread_;
  std::thread detectThread_;
  std::atomic<bool> running_{true};

  const int FRAME_TO_DEC = config_.face_recognition.frame_to_dec;
  int frameCount_ = 0;
};
