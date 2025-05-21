// include/WebSocketServer.h
#pragma once

#include "Types.hpp"
#include <App.h>
#include <vector>

class WebSocketServer {
public:
  WebSocketServer() = default;
  void run();
  void stop();

  std::vector<uWS::WebSocket<false, true, PerSocketData> *> videoClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> faceClients_;

private:
  uWS::App app_;
  uWS::Loop *loop_{nullptr};
};
