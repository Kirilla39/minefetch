#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <minefetch/minecraft_motd.hpp>


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <host> [port]" << std::endl;
        return 1;
    }

    try {
        MinecraftMotd mcm;
        const char* host = argv[1];
        unsigned int port = (argc > 2) ? atoi(argv[2]) : 25565;
        
        std::string motd = mcm.getMotd(host, port);
        std::cout << "MOTD: " << motd << std::endl;
    }
	catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
