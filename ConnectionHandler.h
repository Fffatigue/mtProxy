
#ifndef PROXY_CONNECTIONHANDLER_H
#define PROXY_CONNECTIONHANDLER_H

#include <sys/select.h>
#include <vector>
#include <map>
#include "Connection/Connection.h"
#include "Cache/CacheController.h"
#include <list>

class ConnectionHandler {
    CacheController *cacheController_;
    Connection *connection_;
    std::vector<char> requestBuf;
    const static int MAX_REQUEST_SIZE = 100000;
    int client_sock_;
    pthread_mutex_t mutex_;
    bool done_;
private:

    bool read_request();

public:
    ConnectionHandler(CacheController *cacheController, int client_sock);
    std::string a;
    void run();

    static void *run(void *handler) {
        ConnectionHandler *connectionHandler = (ConnectionHandler *) handler;
        connectionHandler->run();
    }

    void add_connection(size_t endpos);

    bool isDone();

    ~ConnectionHandler();
};


#endif

