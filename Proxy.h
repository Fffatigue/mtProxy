
#ifndef PROXY_PROXY_H
#define PROXY_PROXY_H


#include <sys/select.h>
#include <list>
#include "Server.h"
#include "ConnectionHandler.h"

class Proxy {
    int listen_sockfd_;
    CacheController cacheController_;
    Server server_;
private:
    std::list<ConnectionHandler*> connectionHandlers_ ;

    void process_connections();

public:
    explicit Proxy(int listen_port);

    void run();
};


#endif

