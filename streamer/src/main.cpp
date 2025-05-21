// src/main.cpp
#include "VideoStreamer.hpp"
#include "WebSocketServer.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

int main() {
  std::cout << "                         .___" << std::endl;
  std::cout << "   ____   ___________  __| _/" << std::endl;
  std::cout << "  / ___\\ / ___\\_  __ \\/ __ |" << std::endl;
  std::cout << " / /_/  > /_/  >  | \\/ /_/ |" << std::endl;
  std::cout << " \\___  /\\___  /|__|  \\____ |" << std::endl;
  std::cout << "/_____//_____/            \\/" << std::endl;
  std::cout << std::endl;

  spdlog::set_pattern("[%C-%m-%d %H:%M:%S] [%^%L%$] %v");

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_);

  streamer.start();
  server.run();

  return 0;
}
