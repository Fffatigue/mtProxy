
#include <algorithm>
#include <pthread.h>
#include <csignal>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include "Proxy.h"
#include "Server.h"

Proxy::Proxy(int listen_port) :
        server_(listen_port) {
}

void Proxy::run() {
    signal(SIGPIPE, SIG_IGN);
    while (true) {
        for (std::list<ConnectionHandler *>::iterator it = connectionHandlers_.begin();
             it != connectionHandlers_.end();) {
            if ((*it)->isDone()) {
                delete (*it);
                connectionHandlers_.erase(it++);
            } else {
                it++;
            }
        }
#ifdef DEBUG
        printf("current number of connections : %d\n", connectionHandlers_.size());
#endif
        process_connections();
    }

}

void Proxy::process_connections() {
    int sock;
    pthread_t pthread;
    sock = server_.accept();
    ConnectionHandler *connectionHandler = new ConnectionHandler(&cacheController_, sock);
    int status = pthread_create(&pthread, NULL, ConnectionHandler::run, (void *) connectionHandler);
    if (status != 0) {
        printf("main error: can't create thread, status = %d\n", status);
        delete (connectionHandler);
        return;
    }
    connectionHandlers_.push_back(connectionHandler);
}

Proxy::~Proxy() {
    for (std::list<ConnectionHandler *>::iterator it = connectionHandlers_.begin();
         it != connectionHandlers_.end();){
        delete (*it);
    }
}


