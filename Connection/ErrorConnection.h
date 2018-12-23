
#ifndef PROXY_ERRORCONNECTION_H
#define PROXY_ERRORCONNECTION_H


#include "Connection.h"

class ErrorConnection : public Connection {
public:
    ErrorConnection(int clientfd, std::string errormsg);

    virtual void exchange_data();

    virtual ~ErrorConnection();
};


#endif

