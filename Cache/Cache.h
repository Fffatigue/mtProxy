#ifndef PROXY_CACHE_H
#define PROXY_CACHE_H


#include <vector>
#include <string>

class Cache {
    std::string path_;
    int state_;
    int clientsUsing_;
    std::vector<char> cache_;
    static const int MAX_CACHE_SIZE = 1000000;
    pthread_mutex_t mutex_;
public:

    explicit Cache(std::string &);

    ~Cache();

    enum STATE {
        CACHED, CACHING, NOCACHEABLE, DROPPED
    };

    std::string &getPath();

    int getState();

    int getCache(char *buf, int offset, int length);

    int putCache(char *buf, int length);

    void drop();

    void setCached();

    void markUsing();

    void markNoUsing();

    bool isUsing();
};


#endif

