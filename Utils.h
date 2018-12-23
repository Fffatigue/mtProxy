
#ifndef PROXY_UTILS_H
#define PROXY_UTILS_H

#include <vector>

class Utils {
public:
    static int parse_port(const char *s_port);

    static struct sockaddr_in get_sockaddr(const char *name, int port);

    static std::pair<std::string, int> parsePath(const char *path, int pathlen);

    static void
    makeNewRequest(char *oldRequest, std::vector<char> &newRequest, struct phr_header *headers, int headers_num);

private:
    static void prepare_hints(struct addrinfo *hints);
};


#endif

