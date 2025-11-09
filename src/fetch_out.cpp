#include <minefetch/motd_config.hpp>
#include <minefetch/utils.hpp>
#include <nlohmann/json.hpp>
#include <minefetch/fetch_out.hpp>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>

FetchOut::FetchOut(MotdConfig conf, nlohmann::json result, std::string serverip){
  config = conf;
  oss << "\033[s";
  logoKitty(result["favicon"].get<std::string>().substr(22));
  oss << "\033[u";

  std::string desc;
  std::string lpadding = "\033[" + std::to_string(config.logo.padding.left + config.logo.padding.right + config.logo.height * 2 + 1) + "C";
  
  for(ModulesProperty module : config.modules){
    oss << lpadding << module.key << " ";
    switch (module.type){
      case IP:
		oss << serverip;
        break;

      case DESCRIPTION:
        desc = processDescription(result["description"], module.color);
        {
          size_t pos = desc.find('\n');
          if (pos != std::string::npos) {
            desc.replace(pos, 1, "\n" + lpadding + std::string((module.key.length() - count_if(module.key.begin(), module.key.end(), [](char c)->bool { return (c & 0xC0) == 0x80; })) + 1, ' '));
            
          }
        }
        oss << desc;
        break;

      case ONLINE:
        if (result.contains("players")) {
          std::string formatted = module.format;
          Utils().replaceAll(formatted, "{0}", std::to_string(result["players"]["online"].get<int>()));
          Utils().replaceAll(formatted, "{1}", std::to_string(result["players"]["max"].get<int>()));
          oss << formatted;
        }
        break;
        
      case VERSION:
        if (result.contains("version")) {oss << result["version"]["name"].get<std::string>();}
        break;
    }
    oss << "\r\n";
  }

  std::cout << oss.str();
}

void FetchOut::logoKitty(std::string img_base64){
  for(int i = 0; i < config.logo.padding.top; ++i){
	oss << "\n";
  }

  if (config.logo.padding.left > 0) oss << "\033[" + std::to_string(config.logo.padding.left) + "C";
  int rows = config.logo.height;
  int cols = config.logo.height * 2;
  
  oss << "\033_G"
      << "f=100,t=d,a=T"
      << ",s=" << rows
      << ",v=" << rows
      << ",c=" << cols
      << ",r=" << rows
      << ",m=0;"
      << img_base64 
      << "\033\\";
}

std::string FetchOut::processDescription(const nlohmann::json& description, int colorMode) {
    std::stringstream result;
    
    if (description.is_string()) {
        std::string desc_str = description.get<std::string>();
        if (desc_str.find("\xC2\xA7") != std::string::npos || desc_str.find('&') != std::string::npos) {
            nlohmann::json converted = convertSectionCodesToJson(desc_str);
            if (colorMode == -1) {processTrueColor(converted, result);} else {extractPlainText(converted, result);}
        } else {
            if (colorMode == -1) {
                nlohmann::json basic_desc = nlohmann::json::object({{"text", desc_str}});
                processTrueColor(basic_desc, result);
            } else {
                result << desc_str;
            }
        }
    } else if (colorMode == -1) {processTrueColor(description, result);} else {extractPlainText(description, result);}
    
    return result.str();
}

nlohmann::json FetchOut::convertSectionCodesToJson(const std::string& text) {
    nlohmann::json result = nlohmann::json::array();
    std::string current_text;
    nlohmann::json current_format = nlohmann::json::object();
    
    for (size_t i = 0; i < text.length(); i++) {
        bool is_section_char = false;
        char section_char = '\0';
        
        if (i + 1 < text.length() && static_cast<unsigned char>(text[i]) == 0xC2 && static_cast<unsigned char>(text[i+1]) == 0xA7) {
            is_section_char = true;
            section_char = text[i+1]; 
            i++; 
        }
        else if (text[i] == '&') { is_section_char = true; section_char = text[i];}
        
        if (is_section_char && i + 1 < text.length()) {
            if (!current_text.empty()) {
                nlohmann::json element = current_format;
                element["text"] = current_text;
                result.push_back(element);
                current_text.clear();
            }
            
            char code = text[++i];
            current_format = parseSectionCode(code);
        }
		else {current_text += text[i];}
    }
    
    if (!current_text.empty()) {
        nlohmann::json element = current_format;
        element["text"] = current_text;
        result.push_back(element);
    }
    
    if (result.empty()) {return nlohmann::json::object({{"text", text}});}
    if (result.size() == 1) {return result[0];}
    
    return nlohmann::json::object({{"extra", result}, {"text", ""}});
}

nlohmann::json FetchOut::parseSectionCode(char code) {
    nlohmann::json format = nlohmann::json::object();
    switch (code) {
        case '0': format["color"] = "black"; break;
        case '1': format["color"] = "dark_blue"; break;
        case '2': format["color"] = "dark_green"; break;
        case '3': format["color"] = "dark_aqua"; break;
        case '4': format["color"] = "dark_red"; break;
        case '5': format["color"] = "dark_purple"; break;
        case '6': format["color"] = "gold"; break;
        case '7': format["color"] = "gray"; break;
        case '8': format["color"] = "dark_gray"; break;
        case '9': format["color"] = "blue"; break;
        case 'a': case 'A': format["color"] = "green"; break;
        case 'b': case 'B': format["color"] = "aqua"; break;
        case 'c': case 'C': format["color"] = "red"; break;
        case 'd': case 'D': format["color"] = "light_purple"; break;
        case 'e': case 'E': format["color"] = "yellow"; break;
        case 'f': case 'F': format["color"] = "white"; break;
        case 'k': case 'K': format["obfuscated"] = true; break;
        case 'l': case 'L': format["bold"] = true; break;
        case 'm': case 'M': format["strikethrough"] = true; break;
        case 'n': case 'N': format["underlined"] = true; break;
        case 'o': case 'O': format["italic"] = true; break;
        case 'r': case 'R': format = nlohmann::json::object(); break; 
    }
    return format;
}

void FetchOut::extractPlainText(const nlohmann::json& json, std::stringstream& output) {
    if (json.is_string()) {output << json.get<std::string>();}
	else if (json.is_object()) {
        if (json.contains("text") && json["text"].is_string()) {
		  output << json["text"].get<std::string>();
		}
        if (json.contains("extra") && json["extra"].is_array()) {
            for (const auto& extra : json["extra"]) {extractPlainText(extra, output);}
        }
    } else if (json.is_array()) {
        for (const auto& item : json) {extractPlainText(item, output);}
    }
}

void FetchOut::processTrueColor(const nlohmann::json& json, std::stringstream& output) {
    if (json.is_string()) {output << json.get<std::string>();}
	else if (json.is_object()) {
        if (json.contains("color")) {
            std::string color = json["color"].get<std::string>();
            if (color[0] == '#') {output << convertHexToTrueColor(color);}
			else {output << convertMinecraftColor(color, -1);}
        }
        
        if (json.contains("bold") && json["bold"].get<bool>()) {output << "\033[1m";}
        if (json.contains("underlined") && json["underlined"].get<bool>()) {output << "\033[4m";}
        if (json.contains("italic") && json["italic"].get<bool>()) {output << "\033[3m";}
        if (json.contains("strikethrough") && json["strikethrough"].get<bool>()) {output << "\033[9m";}
        if (json.contains("text") && json["text"].is_string()) {output << json["text"].get<std::string>();}
        
        output << "\033[0m";
        
        if (json.contains("extra") && json["extra"].is_array()) {
            for (const auto& extra : json["extra"]) {processTrueColor(extra, output);}
		}
    } else if (json.is_array()) {for (const auto& item : json) {processTrueColor(item, output);}}
}

std::string FetchOut::convertHexToTrueColor(const std::string& hexColor) {
    if (hexColor.length() != 7 || hexColor[0] != '#') {return "\033[97m";}
    
    int r, g, b;
    sscanf(hexColor.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string FetchOut::convertMinecraftColor(const std::string& mcColor, int colorMode) {
    static std::unordered_map<std::string, std::string> colorMap = {
        {"black", "\033[38;2;0;0;0m"},
        {"dark_blue", "\033[38;2;0;0;170m"},
        {"dark_green", "\033[38;2;0;170;0m"},
        {"dark_aqua", "\033[38;2;0;170;170m"},
        {"dark_red", "\033[38;2;170;0;0m"},
        {"dark_purple", "\033[38;2;170;0;170m"},
        {"gold", "\033[38;2;255;170;0m"},
        {"gray", "\033[38;2;170;170;170m"},
        {"dark_gray", "\033[38;2;85;85;85m"},
        {"blue", "\033[38;2;85;85;255m"},
        {"green", "\033[38;2;85;255;85m"},
        {"aqua", "\033[38;2;85;255;255m"},
        {"red", "\033[38;2;255;85;85m"},
        {"light_purple", "\033[38;2;255;85;255m"},
        {"yellow", "\033[38;2;255;255;85m"},
        {"white", "\033[38;2;255;255;255m"}
    };
    
    if (mcColor[0] == '#') {return convertHexToTrueColor(mcColor);}
    
    auto it = colorMap.find(mcColor);
    if (it != colorMap.end()) {return it->second;}
    
    return "\033[38;2;255;255;255m"; 
}
