

#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include "DirectConnection.h"


void DirectConnection::exchange_data() {
    if(connected_) {
        sendcf();
        recvfc();
    }
    sendfc();

}

DirectConnection::DirectConnection(int client_socket, int forwarding_socket, std::vector<char> &buf_cf,
                                   sockaddr_in *serveraddr) : connected_(),
                                                              forwarding_socket_(forwarding_socket), buf_cf_(),
                                                              serveraddr_(serveraddr),
                                                              data_c_f_(0), cfoffset(0) {
    client_socket_ = client_socket;
    for (int i = 0; i < buf_cf.size(); i++) {
        buf_cf_[i] = buf_cf[i];
    }
    data_c_f_ = buf_cf.size();

    connect();

}

void DirectConnection::connect() {
    int ret;
    ret = ::connect(forwarding_socket_, reinterpret_cast<sockaddr *> (serveraddr_), sizeof(*serveraddr_));
    if (ret == 0) {
        connected_ = true;
        return;
    }
    connected_ = false;
    std::string tmpbuf = "HTTP/1.0 522 Connection Timed Out\r\n\r\n";
    for (int i = 0; i < tmpbuf.size(); i++) {
        buf_fc_[i] = tmpbuf[i];
    }
    data_f_c_ = tmpbuf.length();
}

DirectConnection::~DirectConnection() {
    close(client_socket_);
    close(forwarding_socket_);

}

ssize_t DirectConnection::recvfc() {
    ssize_t ret;
    if (data_f_c_ == 0 && active_) {
        fcoffset_ = 0;
        ret = recv(forwarding_socket_, buf_fc_, MAX_SEND_SIZE, 0);
        if (ret == 0 || ret == -1) {
            active_ = false;
            return ret;
        }
        data_f_c_ = ret;
    }
    return 0;
}

void DirectConnection::sendcf() {
    ssize_t ret;
    while (data_c_f_ != 0 && active_) {
        size_t send_size = data_c_f_ > MAX_SEND_SIZE ? MAX_SEND_SIZE : data_c_f_;
        ret = send(forwarding_socket_, buf_cf_ + cfoffset, send_size, 0);
        data_c_f_ -= ret;
        cfoffset += ret;
        if (ret == -1) {
            active_ = false;
            return;
        }
    }
}

