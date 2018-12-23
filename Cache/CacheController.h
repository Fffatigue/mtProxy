

#ifndef PROXY_CACHECONTROLLER_H
#define PROXY_CACHECONTROLLER_H


#include <string>
#include <list>
#include <map>
#include <set>
#include "Cache.h"

class CacheController {
    typedef std::list<Cache *> CacheQueue;
    typedef std::map<std::string, CacheQueue::iterator> CacheMap;
    const static int MAX_CACHE_ENTRIES = 50;
    CacheMap cacheMap_;
    CacheQueue cacheQueue_;
private:
    bool dropCache();
public:
    Cache *getCache(std::string Path);
};


#endif

