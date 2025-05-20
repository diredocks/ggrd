// include/WebSocketServer.h
#pragma once

#include "Types.hpp"
#include <App.h>
#include <vector>

class WebSocketServer {
public:
  WebSocketServer() {};
  void run();

  std::vector<uWS::WebSocket<false, true, PerSocketData> *> videoClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> faceClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> msgClients_;

private:
  uWS::App app_;
};
