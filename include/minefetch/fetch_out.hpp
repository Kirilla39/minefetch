#pragma once
#include <minefetch/motd_config.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

class FetchOut {
private:
    std::ostringstream oss;
    MotdConfig config;
    void logoKitty(std::string img_base64);
    std::string processDescription(const nlohmann::json& description, int colorMode);
    void extractPlainText(const nlohmann::json& json, std::stringstream& output);
    void processTrueColor(const nlohmann::json& json, std::stringstream& output);
    nlohmann::json convertSectionCodesToJson(const std::string& text);
    nlohmann::json parseSectionCode(char code);
    std::string convertHexToTrueColor(const std::string& hexColor);
    std::string convertMinecraftColor(const std::string& mcColor, int colorMode = -1);
public:
    FetchOut(MotdConfig conf, nlohmann::json result, std::string serverip = " ");
    void motdText();
};
