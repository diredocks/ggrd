// include/VideoStreamer.h
#pragma once

#include "Types.hpp"
#include <App.h>
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
  ~VideoStreamer() {
    captureThread_.join();
    faceThread_.join();
  };
  void start() {
    captureThread_ = std::thread(&VideoStreamer::captureLoop, this);
    faceThread_ = std::thread(&VideoStreamer::faceDetectLoop, this);
  };

private:
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
  std::thread faceThread_;
};
