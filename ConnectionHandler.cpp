
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <list>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include "third-party/picohttpparser.h"
#include "ConnectionHandler.h"
#include "Connection/DirectConnection.h"
#include "Utils.h"
#include "Connection/ErrorConnection.h"
#include "Connection/CachingConnection.h"
#include "Connection/CachedConnection.h"


void ConnectionHandler::add_connection(size_t requestEndpos) {
    const char *path;
    const char *method;
    int minor_version;
    struct phr_header headers[100];
    size_t prevbuflen = 0, method_len, path_len, num_headers;
    num_headers = sizeof(headers) / sizeof(headers[0]);
    char oldRequest[requestEndpos];
    std::vector<char> newRequest;
    for (int i = 0; i < requestEndpos; i++) {
        oldRequest[i] = requestBuf[i];
    }
    phr_parse_request(oldRequest, requestEndpos, &method, &method_len, &path, &path_len,
                      &minor_version, headers, &num_headers, prevbuflen);
    //if not http 1.0
    if (minor_version != 0) {
        connection_ = new ErrorConnection(client_sock_, "HTTP/1.0 505 HTTP Version Not Supported\r\n\r\n");
        return;
    }
    //if method GET
    if (!strncmp(method, "GET", 3)) {
        Utils::makeNewRequest(oldRequest, newRequest, headers, num_headers);
        std::pair<std::string, int> url_port;
        url_port.second = -2;
        for (int i = 0; i < num_headers; i++) {
            if (!strncmp(headers[i].name, "Host", headers[i].name_len < 4 ? headers[i].name_len : 4)) {
                url_port = Utils::parsePath(headers[i].value, headers[i].value_len);
                break;
            }
        }
        //if no HOST param
        if (url_port.second == -2) {
#ifdef DEBUG
            printf("Error connection 404\n");
#endif
            connection_ = new ErrorConnection(client_sock_, "HTTP/1.0 404 Not Found\r\n\r\n");
            return;
        }
        Cache *cache = cacheController_->getCache(std::string(path, path_len));

        //if cache now unavailable
        if (cache == NULL || cache->getState() == Cache::CACHING || cache->getState() == Cache::NOCACHEABLE) {
            int forwarding_sock = socket(AF_INET, SOCK_STREAM, 0);
            int forward_port = url_port.second == -1 ? 80 : url_port.second;
            sockaddr_in forwaddr;
            try {
                forwaddr = Utils::get_sockaddr(url_port.first.c_str(), forward_port);
            } catch (std::invalid_argument &e) {
                close(client_sock_);
                close(forwarding_sock);
                return;
            }
            //if cache now caching or nocacheable
            if (cache == NULL || cache->getState() == Cache::NOCACHEABLE) {
#ifdef DEBUG
                printf("Direct connection to %.*s\n", (int) path_len, path);
#endif
                connection_ = new DirectConnection(client_sock_, forwarding_sock, newRequest,
                                                   &forwaddr);
            } else {
#ifdef DEBUG
                printf("Caching connection to  %.*s\n", (int) path_len, path);
#endif
                connection_ = new CachingConnection(client_sock_, forwarding_sock, newRequest, &forwaddr, cache);
            }
            return;
        } else {
            connection_ = new CachedConnection(client_sock_, cache);
#ifdef DEBUG
            printf("Cached connection to %.*s\n", (int) path_len, path);
#endif
        }
    } else {
#ifdef DEBUG
        printf("Error connection 501\n");
#endif
        connection_ = new ErrorConnection(client_sock_, "HTTP/1.0 501 Not Implemented\r\n\r\n");
    }
}

bool ConnectionHandler::read_request() {
    size_t endpos = -1;
    char buf[MAX_REQUEST_SIZE];
    ssize_t len = recv(client_sock_, buf, MAX_REQUEST_SIZE, 0);
    if (len == 0 || len == -1) {
        //TODO connection off
        stop_ = true;
        return true;
    }
    for (ssize_t i = 0; i < len; i++) {
        if (i < len - 3) {
            if (buf[i] == '\r' && buf[i + 2] == '\r' && buf[i + 3] == '\n' && buf[i + 1] == '\n') {
                endpos = i + 3;
            }
        }
        requestBuf.push_back(buf[i]);
    }
    if (endpos != -1) {
        add_connection(endpos);
        return true;
    }
    return false;
}

ConnectionHandler::ConnectionHandler(CacheController *cacheController, int client_sock) : cacheController_(
        cacheController), client_sock_(client_sock), done_(false),stop_(false), connection_(NULL) {
    pthread_mutex_init(&mutex_, NULL);
}


void ConnectionHandler::run() {
    while (!read_request());
    if (!stop_ && connection_ != NULL) {
        while (connection_->isActive()) {
            connection_->exchange_data();
        }
        delete (connection_);
        connection_ = NULL;
    }
        pthread_mutex_lock(&mutex_);
        done_ = true;
        pthread_mutex_unlock(&mutex_);
}

ConnectionHandler::~ConnectionHandler() {
    pthread_mutex_destroy(&mutex_);
    if(connection_!=NULL){
        delete(connection_);
    }
}

bool ConnectionHandler::isDone() {
    pthread_mutex_lock(&mutex_);
    bool done = done_;
    pthread_mutex_unlock(&mutex_);
    return done;
}

void *ConnectionHandler::run(void *handler) {
    pthread_detach(pthread_self());
    ConnectionHandler *connectionHandler = (ConnectionHandler *) handler;
    connectionHandler->run();
}


