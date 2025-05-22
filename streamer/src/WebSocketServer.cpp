// src/WebSocketServer.cpp
#include "WebSocketServer.hpp"
#include "ConfigManager.hpp"
#include <spdlog/spdlog.h>

void WebSocketServer::run() {
  loop_ = uWS::Loop::get();
  const auto &config = ConfigManager::getInstance().getConfig();

  app_.ws<PerSocketData>(
          "/stream",
          {.compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR |
                                               uWS::DEDICATED_DECOMPRESSOR),
           .open =
               [this](auto *ws) {
                 videoClients_.push_back(ws);
                 spdlog::info("new capture connection! total: {0:d}",
                              videoClients_.size());
               },
           .close =
               [this](auto *ws, int code, std::string_view message) {
                 videoClients_.erase(std::remove(videoClients_.begin(),
                                                 videoClients_.end(), ws),
                                     videoClients_.end());
                 spdlog::info("capture client disconnected, remaining: {0:d}",
                              videoClients_.size());
               }})
      .ws<PerSocketData>(
          "/face",
          {.open =
               [this](auto *ws) {
                 faceClients_.push_back(ws);
                 spdlog::info("face client connected, total:  {0:d}",
                              faceClients_.size());
               },
           .close =
               [this](auto *ws, int, std::string_view) {
                 faceClients_.erase(
                     std::remove(faceClients_.begin(), faceClients_.end(), ws),
                     faceClients_.end());
                 spdlog::info("face client disconnected, remaining: {0:d}",
                              faceClients_.size());
               }})
      .listen(config.server.port,
              [](auto *listen_socket) {
                if (listen_socket) {
                  spdlog::info(
                      "streamer listening at :{}",
                      ConfigManager::getInstance().getConfig().server.port);
                }
              })
      .run();
}

void WebSocketServer::stop() {
  if (loop_) {
    // Close all WebSocket connections
    for (auto *ws : videoClients_) {
      ws->end();
    }
    for (auto *ws : faceClients_) {
      ws->end();
    }
    videoClients_.clear();
    faceClients_.clear();
  }
}
