// src/ConfigManager.cpp
#include "ConfigManager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

bool ConfigManager::loadConfig(const std::string &configPath) {
  try {
    std::ifstream file(configPath);
    if (!file.is_open()) {
      spdlog::error("opening config file: {}", configPath);
      return false;
    }
    nlohmann::json j;
    file >> j;

    // server config
    if (j.contains("server")) {
      auto &server = j["server"];
      if (server.contains("port")) {
        config_.server.port = server["port"];
      }
    }

    // face recognition config
    if (j.contains("face_recognition")) {
      auto &fr = j["face_recognition"];
      if (fr.contains("model_path")) {
        config_.face_recognition.model_path = fr["model_path"];
      }
      if (fr.contains("face_recognition_model_path")) {
        config_.face_recognition.face_recognition_model_path =
            fr["face_recognition_model_path"];
      }
      if (fr.contains("known_faces_folder")) {
        config_.face_recognition.known_faces_folder = fr["known_faces_folder"];
      }
      if (fr.contains("best_known_threshold")) {
        config_.face_recognition.best_known_threshold =
            fr["best_known_threshold"];
      }
      if (fr.contains("distance_threshold")) {
        config_.face_recognition.distance_threshold = fr["distance_threshold"];
      }
      if (fr.contains("max_lost_count")) {
        config_.face_recognition.max_lost_count = fr["max_lost_count"];
      }
      if (fr.contains("frame_to_dec")) {
        config_.face_recognition.frame_to_dec = fr["frame_to_dec"];
      }
    }

    spdlog::info("configuration loaded from {}", configPath);
    return true;
  } catch (const std::exception &e) {
    spdlog::error("loading config: {}", e.what());
    return false;
  }
}
