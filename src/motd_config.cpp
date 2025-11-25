#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <minefetch/motd_config.hpp>
#include <filesystem>
#include <ostream>
#include <string>

MotdConfig::MotdConfig(std::string path){
  if (path.find("~/") == 0) { path = std::string(getenv("HOME")) + path.substr(1); }
  std::ifstream file(std::filesystem::absolute(path));

  if (!file.is_open()) { 
    generateDefault(); file.open(std::filesystem::absolute(path));
    if (!file.is_open()) { throw std::runtime_error("Cannot open config file: " + path); }
  }

  nlohmann::json config_json = nlohmann::json::parse(file);
  file.close();

  if (config_json.contains("serverAddress")) {default_ip = config_json["serverAddress"].get<std::string>();}
  if (config_json.contains("logo")) {
	auto logo_json = config_json["logo"];
	if (logo_json.contains("mode")) logo.mode = logo_json["mode"].get<uint8_t>();
	if (logo_json.contains("color")) logo.color = logo_json["color"].get<int8_t>();
	if (logo_json.contains("height")) logo.height = logo_json["height"].get<uint8_t>();
	if (logo_json.contains("padding")) {
	  auto padding_json = logo_json["padding"];
	  if (padding_json.contains("top")) logo.padding.top = padding_json["top"].get<uint8_t>();
	  if (padding_json.contains("left")) logo.padding.left = padding_json["left"].get<uint8_t>();
	  if (padding_json.contains("right")) logo.padding.right = padding_json["right"].get<uint8_t>();
	}
  }

  if (config_json.contains("modules")) {
	  modules.clear();
	  for (auto module_json : config_json["modules"]) {
		  ModulesProperty module;
		  
		  if (module_json.contains("key")) module.key = module_json["key"].get<std::string>();
		  if (module_json.contains("format")) module.format = module_json["format"].get<std::string>();
		  if (module_json.contains("color")) module.color = module_json["color"].get<int8_t>();
		  if (module_json.contains("type")) {
			  std::string type_str = module_json["type"].get<std::string>();
			  if (type_str == "ip") module.type = IP;
			  else if (type_str == "description") module.type = DESCRIPTION;
			  else if (type_str == "version") module.type = VERSION;
			  else if (type_str == "online") module.type = ONLINE;
			  else if (type_str == "break") module.type = BREAK;
		  }
		  modules.push_back(module);

	  }
  }
}

void MotdConfig::generateDefault(){
  std::filesystem::path cpath = std::string(getenv("HOME")) + "/.config/minefetch/config.json";
  std::filesystem::create_directories(cpath.parent_path()); 
  std::ofstream file(cpath);
  if (file.is_open()) {
	  file << R"({
  "logo": {
	"mode": 1,
	"color": -1,
	"height": 3,
	"padding": {
	  "top": 0,
	  "left": 0,
	  "right": 1
	}
  },
  "modules": [
	{
	  "type": "ip",
	  "key": " IP:"
	},
	{
	  "type": "description", 
	  "key": "󰍳 Description:",
	  "color": -1
	},
	{
	  "type": "break"
	},
	{
	  "type": "online",
	  "format": "{0}/{1}",
	  "key": " Online:"
	},
	{
	  "type": "version",
	  "key": " Version:"
	}
  ]
})";
	  file.close();
  }
}
