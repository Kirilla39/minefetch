#include <iostream>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <minefetch/fetch_out.hpp>
#include <minefetch/minecraft_motd.hpp>


int main(int argc, char* argv[]) {
	MotdConfig oconf = MotdConfig();	
    if (argc < 2 && oconf.default_ip.empty()) {
        std::cout << "Usage: " << argv[0] << " <host> [port]" << std::endl;
        return 1;
    }

    try {
        MinecraftMotd mcm;
        const char* host = (argc > 1) ? argv[1] : oconf.default_ip.c_str();
        unsigned int port = (argc > 2) ? atoi(argv[2]) : 25565;
		
        std::string motd = mcm.getMotd(host, port);
		FetchOut fetchOut = FetchOut(oconf, nlohmann::json::parse(motd), std::string(host));
    }
	catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
