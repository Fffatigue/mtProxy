
#ifndef PROXY_DIRECTCONNECTION_H
#define PROXY_DIRECTCONNECTION_H


#include <netinet/in.h>
#include "Connection.h"

class DirectConnection : public Connection {
protected:
    bool connected_;
    int forwarding_socket_;
    char buf_cf_[MAX_SEND_SIZE];
    ssize_t data_c_f_;
    ssize_t cfoffset;
    sockaddr_in *serveraddr_;

    void connect();

    ssize_t recvfc();

    void sendcf();

public:

    DirectConnection(int client_socket, int forwarding_socket, std::vector<char>& buf_cf, sockaddr_in *serveraddr);

    virtual void exchange_data();

    virtual ~DirectConnection();
};


#endif

