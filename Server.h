
#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H


class Server {
    int listen_sockfd_;
public:
    explicit Server(int listen_port);

    ~Server();

    int accept();

    int get_listen_sockfd();
};


#endif

