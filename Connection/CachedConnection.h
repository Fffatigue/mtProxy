
#ifndef PROXY_CACHEDCONNECTION_H
#define PROXY_CACHEDCONNECTION_H


#include "Connection.h"

class CachedConnection : public Connection {
    int offset_;
    Cache *cache_;
public:
    CachedConnection(int client_socket, Cache *cache);

    virtual void exchange_data();

    virtual ~CachedConnection();
};


#endif

