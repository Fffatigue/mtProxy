cmake_minimum_required(VERSION 3.10)
project(mtProxy)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-D DEBUG -pthread")
add_executable(mtProxy main.cpp Utils.cpp Utils.h Proxy.cpp Proxy.h Server.cpp Server.h ConnectionHandler.cpp ConnectionHandler.h Connection/Connection.h Connection/DirectConnection.cpp Connection/DirectConnection.h Cache/Cache.cpp Cache/Cache.h third-party/picohttpparser.c third-party/picohttpparser.h Connection/ErrorConnection.cpp Connection/ErrorConnection.h Cache/CacheController.cpp Cache/CacheController.h Connection/CachingConnection.h Connection/CachingConnection.cpp Connection/CachedConnection.h Connection/CachedConnection.cpp)