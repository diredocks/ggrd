// src/main.cpp
#include "VideoStreamer.hpp"
#include "WebSocketServer.hpp"
#include "spdlog/spdlog.h"

int main() {
  spdlog::set_pattern("[%C-%m-%d %H:%M:%S] [%^%L%$] %v");

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_,
                         server.msgClients_);

  streamer.start();
  server.run();

  return 0;
}
