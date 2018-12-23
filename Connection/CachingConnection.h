
#ifndef PROXY_CACHINGCONNECTION_H
#define PROXY_CACHINGCONNECTION_H


#include "Connection.h"
#include "DirectConnection.h"

class CachingConnection : public DirectConnection {
    Cache *cache_;
    bool isCacheable_;
public:

    void exchange_data();

    CachingConnection(int client_socket, int forwarding_socket, std::vector<char>& buf_cf, sockaddr_in *serveraddr,
                      Cache *cache);

    virtual ~CachingConnection();
};


#endif

