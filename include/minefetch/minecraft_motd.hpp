#pragma once
#include <cstdint>
#include <string>
#include <vector>

class MinecraftMotd{
private:
  uint64_t packVarInt(uint32_t number);
  uint64_t unpackVarInt(int sock, int* valread);
  uint8_t bytesLength(uint32_t number);
  void packByteToData(uint64_t dataByte, std::vector<uint8_t> &data);

public:
  std::string getMotd(const char* host, unsigned int port = 25565);
};
