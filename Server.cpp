
#include <sys/select.h>
#include <sys/socket.h>
#include <stdexcept>
#include <list>
#include <iostream>
#include <cerrno>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#include "Server.h"
#include "Utils.h"

Server::Server(int listen_port) :
        listen_sockfd_(socket(AF_INET, SOCK_STREAM, 0)) {
    if (listen_sockfd_ == -1) {
        throw std::runtime_error(std::string("socket: ") + strerror(errno));
    }
    if (listen_sockfd_ >= FD_SETSIZE) {
        throw std::runtime_error(std::string("Socket number out of range for select"));
    }
    int opt = 1;
    setsockopt(listen_sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in listenaddr = Utils::get_sockaddr(NULL, listen_port);

    if (bind(listen_sockfd_, reinterpret_cast<struct sockaddr *>(&listenaddr), sizeof(listenaddr))) {
        throw std::runtime_error(std::string("bind: ") + strerror(errno));
    }

    if (listen(listen_sockfd_, SOMAXCONN)) {
        throw std::runtime_error(std::string("listen: ") + strerror(errno));
    }
}

int Server::accept() {
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_sock = ::accept(listen_sockfd_, reinterpret_cast<sockaddr *>(&client_addr), &addrlen);
    if (client_sock < 0) {
        throw std::runtime_error(std::string("accept: ") + strerror(errno));
    }
    return client_sock;
}

int Server::get_listen_sockfd() {
    return listen_sockfd_;
}

Server::~Server() {
    close(listen_sockfd_);
}

