#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <minefetch/minecraft_motd.hpp>
#include <stdexcept>
#include <cstring>


uint64_t MinecraftMotd::packVarInt(uint32_t number){
  uint64_t varint = 0;
  
  while (true) {
	varint <<= 8;
	uint8_t temp = number & 0x7F;
	number >>= 7;
	varint+=temp;
	if(number != 0){ varint |= 0x80; }
	else break;
  }
  return varint;
}

uint64_t MinecraftMotd::unpackVarInt(int sock, int* valread){
  uint64_t varint = 0;
  uint8_t temp = 0x80;
  uint8_t i = 0;

  while (temp & 0x80) {
	*valread = read(sock, &temp, 1);
	if(*valread <= 0){return 0;}
	varint |= (temp & 0x7F) << (7*i);
	i++;
  }
  return varint;
}


uint8_t MinecraftMotd::bytesLength(uint32_t number){
  if(number == 0) return 1;
  for(uint8_t i = 4; i > 0; --i){
	if(number & 0xFF000000){
	  return i;
	}
	else {
	  number<<=8;
	}
  }
  return 0;
} 



void MinecraftMotd::packByteToData(uint64_t dataByte, std::vector<uint8_t> &data){
  for(int64_t i = bytesLength(dataByte)-1; i >= 0; --i){
	data.push_back(dataByte >> (i*8) & 0xFF);
  }
}


std::string MinecraftMotd::getMotd(const char* host, unsigned int port){
  int sock = 0;
  struct sockaddr_in serv_addr; 
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Socket creation failed");
  }

  struct hostent* hostp = gethostbyname(host);
  if(!hostp){
    close(sock);
    throw std::runtime_error("Host resolution failed");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port); 
  memcpy(&serv_addr.sin_addr.s_addr, hostp->h_addr_list[0], hostp->h_length); 

  if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))){
    throw std::runtime_error("Connection failed");
  }

  uint32_t protocol_ver_varint = packVarInt(760);
  uint8_t packet_id = 0;
  uint32_t server_adress_size = strlen(host);
  uint8_t next_state = packVarInt(1);
  uint32_t request_length = bytesLength(packet_id) + bytesLength(protocol_ver_varint) + bytesLength(server_adress_size) + server_adress_size + 3;
  uint32_t total_length = bytesLength(request_length) + request_length;

  std::vector<uint8_t> data;
  data.reserve(total_length);

  packByteToData(packVarInt(request_length), data);
  packByteToData(packet_id, data);
  packByteToData(protocol_ver_varint, data);
  packByteToData(server_adress_size, data);
  data.insert(data.end(), host, host + server_adress_size);
  packByteToData(htons(port), data);
  packByteToData(next_state, data);

  uint8_t status_data[2] = {1,0};

  send(sock, data.data(), data.size(), 0);
  send(sock, status_data, 2, 0);
 
  int valread;
  uint64_t packet_length = unpackVarInt(sock, &valread);
  if(valread <= 0) {
    close(sock);
    throw std::runtime_error("Failed to read packet length");
  }

  uint64_t response_packet_id = unpackVarInt(sock, &valread);
  if(valread <= 0 || response_packet_id != 0) {
    close(sock);
    throw std::runtime_error("Invalid response packet ID");
  }

  uint64_t stringLength = unpackVarInt(sock, &valread);
  if(valread <= 0) {
    close(sock);
    throw std::runtime_error("Failed to read string length");
  }

  std::vector<char> buffer(stringLength);
  
  size_t totalRead = 0;
  while(totalRead < stringLength) {
    valread = read(sock, buffer.data() + totalRead, stringLength - totalRead);
    if(valread <= 0) {
      close(sock);
      throw std::runtime_error("Failed to read complete response");
    }
    totalRead += valread;
  }

  close(sock);
  return std::string(buffer.data(), stringLength); 
}
