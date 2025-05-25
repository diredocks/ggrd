// include/ConfigManager.hpp
#pragma once

#include <string>

struct Config {
  struct {
    int port = 9001;
  } server;

  struct {
    std::string model_path = "shape_predictor_68_face_landmarks.dat";
    std::string face_recognition_model_path =
        "dlib_face_recognition_resnet_model_v1.dat";
    std::string known_faces_folder = "known_faces";
    double best_known_threshold = 0.5;
    double distance_threshold = 100.0;
    int max_lost_count = 5;
    int frame_to_dec = 5;
  } face_recognition;
};

class ConfigManager {
public:
  static ConfigManager &getInstance() {
    static ConfigManager instance;
    return instance;
  }

  bool loadConfig(const std::string &configPath);

  const Config &getConfig() const { return config_; }

private:
  ConfigManager() = default;
  Config config_;
};
