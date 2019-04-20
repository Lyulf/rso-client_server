#ifndef RSO_DEFAULTS_H
#define RSO_DEFAULTS_H

#include <string>
#include <netinet/in.h>

namespace rso {
namespace defaults {

const int domain = AF_INET;
const std::string address = "127.0.0.1";
const short port = 8888;
const size_t max_clients = 10;
const int max_queue = 3;
}
}

#endif