// src/WebSocketServer.cpp
#include "WebSocketServer.hpp"
#include "spdlog/spdlog.h"

void WebSocketServer::run() {
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
      .ws<PerSocketData>(
          "/msg",
          {.open =
               [this](auto *ws) {
                 msgClients_.push_back(ws);
                 spdlog::info("msg client connected, total:  {0:d}",
                              msgClients_.size());
               },
           .close =
               [this](auto *ws, int, std::string_view) {
                 msgClients_.erase(
                     std::remove(msgClients_.begin(), msgClients_.end(), ws),
                     msgClients_.end());
                 spdlog::info("msg client disconnected, remaining: {0:d}",
                              msgClients_.size());
               }})
      .listen(9001,
              [](auto *listen_socket) {
                if (listen_socket) {
                  spdlog::info("server listening at :9001");
                }
              })
      .run();
}
