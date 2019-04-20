#ifndef RSO_CLIENT_DISCONNECTED_H
#define RSO_CLIENT_DISCONNECTED_H

#include <stdexcept>

class DisconnectedException : public std::runtime_error {
public:
  DisconnectedException(const char* msg) : runtime_error(msg) { }
};

#endif