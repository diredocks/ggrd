// src/main.cpp
#include "VideoStreamer.hpp"
#include "WebSocketServer.hpp"
#include "spdlog/spdlog.h"
#include <csignal>
#include <cstdlib>
#include <iostream>

static WebSocketServer *g_server = nullptr;
static VideoStreamer *g_streamer = nullptr;

void signalHandler(int signum) {
  std::cout << "\b\b"; // hide ^C
  spdlog::info("shutting down...", signum);

  if (g_streamer) {
    g_streamer->stop();
  }

  if (g_server) {
    g_server->stop();
  }

  exit(EXIT_SUCCESS);
}

int main() {
  std::cout << R"(
                         .___
   ____   ___________  __| _/
  / ___\ / ___\_  __ \/ __ | 
 / /_/  > /_/  >  | \/ /_/ | 
 \___  /\___  /|__|  \____ | 
/_____//_____/            \/ [s]
  )" << std::endl;
  std::cout << std::endl;

  spdlog::set_pattern("[%C-%m-%d %H:%M:%S] [%^%L%$] %v");

  // Set up signal handlers
  signal(SIGINT, signalHandler);

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_);

  // Store global pointers for signal handler
  g_server = &server;
  g_streamer = &streamer;

  streamer.start();
  server.run();

  // Clean up on normal exit
  streamer.stop();
  server.stop();

  return 0;
}
