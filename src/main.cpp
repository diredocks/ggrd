// src/main.cpp
#include "VideoStreamer.h"
#include "WebSocketServer.h"
#include "spdlog/spdlog.h"

int main() {
  spdlog::set_pattern("[%C-%m-%d %H:%M:%S] [%^%L%$] %v");

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_);

  streamer.start();
  server.run();

  return 0;
}
