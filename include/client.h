#ifndef RSO_CLIENT_H
#define RSO_CLIENT_H

#include <memory>
#include "defaults.h"
#include "message.h"

namespace rso {

class Client {
public:
    Client(const std::string& address, const short port = defaults::port);
    ~Client();

    void sendSqrtRequest(double value);
    void sendDateRequest();
    Message receiveRequest();

private:
    struct ClientImpl;
    std::unique_ptr<ClientImpl> impl;
};

}

#endif