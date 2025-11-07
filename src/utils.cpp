#include <algorithm>
#include <cstddef>
#include <minefetch/utils.hpp>
#include <string>

std::string Utils::replaceAll(std::string &str, std::string substr, std::string replacement){
  std::size_t pos = str.find(substr);
  while(pos != std::string::npos){
	str.replace(pos,substr.length(),replacement);
	pos = str.find(substr);
  }
  return str;  
}
