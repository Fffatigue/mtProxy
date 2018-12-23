#include <iostream>
#include <cstdlib>
#include "Utils.h"
#include "Proxy.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Invalid arguments! Usage: " << argv[0] << " listen_port" << std::endl;
        exit(EXIT_SUCCESS);
    }
    try {
        int listen_port = Utils::parse_port(argv[1]);
        Proxy proxy(listen_port);
        proxy.run();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

