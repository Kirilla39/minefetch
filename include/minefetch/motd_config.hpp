#pragma once
#include <cstdint>
#include <string>
#include <vector>


struct Logo{
  uint8_t mode;
  int8_t color;
  uint8_t height;
  struct{
	uint8_t top;
	uint8_t left;
	uint8_t right;
  } padding;

  Logo() = default;
};

enum ModulesType{
  IP,
  DESCRIPTION,
  VERSION,
  ONLINE,
  BREAK,
};

struct ModulesProperty{
  std::string key;
  std::string format;
  ModulesType type;
  int8_t color;

  ModulesProperty() = default;
};

class MotdConfig{
public:
  Logo logo;
  std::vector<ModulesProperty> modules;
  std::string default_ip;
  MotdConfig(std::string path = "~/.config/minefetch/config.json");
  void setIp(std::string address);
  void generateDefault();
};
