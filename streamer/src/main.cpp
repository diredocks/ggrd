// src/main.cpp
#include "ConfigManager.hpp"
#include "VideoStreamer.hpp"
#include "WebSocketServer.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>

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

int main(int argc, char *argv[]) {
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

  std::string configPath = "config.json";
  if (argc > 1) {
    configPath = argv[1];
  }

  if (!ConfigManager::getInstance().loadConfig(configPath)) {
    spdlog::error("loading configuration from {}", configPath);
    return 1;
  }

  signal(SIGINT, signalHandler);

  WebSocketServer server;
  VideoStreamer streamer(server.videoClients_, server.faceClients_);
  g_server = &server;
  g_streamer = &streamer;

  streamer.start();
  server.run();
  return 0;
}
