// include/WebSocketServer.h
#pragma once

#include "Types.h"
#include <App.h>
#include <vector>

class WebSocketServer {
public:
  WebSocketServer() {};
  void run();

  std::vector<uWS::WebSocket<false, true, PerSocketData> *> videoClients_;
  std::vector<uWS::WebSocket<false, true, PerSocketData> *> faceClients_;

private:
  uWS::App app_;
};
