// include/VideoStreamer.h
#pragma once

#include "Types.h"
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
  void captureLoop();    // 采集 + 推视频流
  void faceDetectLoop(); // 专门做识别 + 推 json

  // 客户端列表
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> &videoClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> &faceClients_;

  // 队列与同步原语
  std::deque<cv::Mat> frameQueue_;
  std::mutex queueMutex_;
  std::condition_variable queueCond_;
  const size_t maxQueueSize_ = 5; // 队列长度上限*/

  cv::VideoCapture cap_;
  std::thread captureThread_;
  std::thread faceThread_;
};
