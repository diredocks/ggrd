// src/main.cpp
#include "VideoStreamer.hpp"
#include "WebSocketServer.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

int main() {
  std::cout << R"(
                         .___
   ____   ___________  __| _/
  / ___\ / ___\_  __ \/ __ | 
 / /_/  > /_/  >  | \/ /_/ | 
 \___  /\___  /|__|  \____ | 
/_____//_____/            \/ 
  )" << std::endl;
  std::cout << std::endl;

  spdlog::set_pattern("[%C-%m-%d %H:%M:%S] [%^%L%$] %v");

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_);

  streamer.start();
  server.run();

  return 0;
}
