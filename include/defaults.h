#ifndef RSO_DEFAULTS_H
#define RSO_DEFAULTS_H

#include <string>
#include <netinet/in.h>

namespace rso {
namespace defaults {

constexpr int domain = AF_INET;
constexpr char address[] = "127.0.0.1";
constexpr short port = 8888;
constexpr size_t max_clients = 10;
constexpr int max_queue = 3;
constexpr std::size_t buffer_size = 1024;

}
}

#endif