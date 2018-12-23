
#include <string>
#include "ErrorConnection.h"

ErrorConnection::ErrorConnection(int clientfd, std::string errormsg)     {
    for (int i = 0; i < errormsg.length(); i++) {
        buf_fc_[i] = errormsg[i];
    }
    client_socket_ = clientfd;
    data_f_c_ = errormsg.length();
}

void ErrorConnection::exchange_data() {
    sendfc();
    if(data_f_c_ == 0){
        active_ = false;
    }
}

ErrorConnection::~ErrorConnection() {}

