
#include <cstdio>
#include <cstdlib>
#include "Cache.h"

int Cache::getState() {
    pthread_mutex_lock(&mutex_);
    int state = state_;
    pthread_mutex_unlock(&mutex_);
    return state;
}

int Cache::getCache(char *buf, int offset, int length) {
    if (state_ != CACHED) {
        return -1;
    }
    length = (length + offset < cache_.size()) ? length + offset : cache_.size();
    for (int i = offset; i < length; i++) {
        buf[i - offset] = cache_[i];
    }
    return length - offset;
}

 int Cache::putCache(char *buf, int length) {
     pthread_mutex_lock(&mutex_);
    int state = state_;
     pthread_mutex_unlock(&mutex_);
    if (state_ != CACHING) {
        return state;
    }
    if (length + cache_.size() > MAX_CACHE_SIZE) {
        pthread_mutex_lock(&mutex_);
        state_ = NOCACHEABLE;
        pthread_mutex_unlock(&mutex_);
        return NOCACHEABLE;
    }
    for (int i = 0; i < length; i++) {
        cache_.push_back(buf[i]);
    }
    return state;
}

void Cache::setCached() {
    pthread_mutex_lock(&mutex_);
    state_ = CACHED;
    pthread_mutex_unlock(&mutex_);
}

Cache::Cache(std::string &path) : state_(CACHING), clientsUsing_(0), path_(path) {
    pthread_mutex_init(&mutex_, NULL);
};

void Cache::markUsing() {
    pthread_mutex_lock(&mutex_);
    clientsUsing_++;
    pthread_mutex_unlock(&mutex_);
}

void Cache::markNoUsing() {
    pthread_mutex_lock(&mutex_);
    clientsUsing_--;
    pthread_mutex_unlock(&mutex_);
}

bool Cache::isUsing() {
    pthread_mutex_lock(&mutex_);
    int clientsUsing = clientsUsing_;
    pthread_mutex_unlock(&mutex_);
    return !clientsUsing == 0;
}

void Cache::drop() {
    pthread_mutex_lock(&mutex_);
    state_ = DROPPED;
    pthread_mutex_unlock(&mutex_);
}

std::string &Cache::getPath() {
    return path_;
}

Cache::~Cache() {
    pthread_mutex_destroy(&mutex_);
}

