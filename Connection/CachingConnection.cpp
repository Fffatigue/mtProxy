
#include <cstdio>
#include "CachingConnection.h"


void CachingConnection::exchange_data() {
    if (connected_) {
        if (active_) {
            sendfc();
            if (!active_) {
                cache_->drop();
            }
        }
        if (active_) {
            sendcf();
            if (!active_) {
                cache_->drop();
                isCacheable_ = false;
            }
        }
    }
    if (active_) {
        int ret = recvfc();
        if (!active_ && isCacheable_) {
            if (ret) {
                cache_->drop();
                isCacheable_ = false;
            } else {
                cache_->setCached();
            }
        }
        if (active_ && isCacheable_) {
            if (cache_->putCache(buf_fc_, data_f_c_) != Cache::CACHING) {
                isCacheable_ = false;
            }
        }
    }

}

CachingConnection::CachingConnection(int client_socket, int forwarding_socket, std::vector<char> &buf_cf,
                                     sockaddr_in *serveraddr, Cache *cache) : DirectConnection(client_socket,
                                                                                               forwarding_socket,
                                                                                               buf_cf,
                                                                                               serveraddr),
                                                                              cache_(cache), isCacheable_(true) {
    if (!connected_) {
        cache->drop();
        isCacheable_ = false;
    }
}

CachingConnection::~CachingConnection() {
    cache_->markNoUsing();
}

