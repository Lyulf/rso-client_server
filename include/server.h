#ifndef RSO_SERVER_H
#define RSO_SERVER_H

#include <cstddef>
#include <memory>
#include "defaults.h"

namespace rso {

class Server {
public:
    Server(
        const short port = defaults::port,
        const std::size_t max_clients = defaults::max_clients,
        const int max_queue = defaults::max_queue
    );
    ~Server();
    void acceptClients();

private:
    struct ServerImpl;
    std::unique_ptr<ServerImpl> impl;
};

}

#endif