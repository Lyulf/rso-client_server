#ifndef RSO_COMMON_H
#define RSO_COMMON_H

#include <cstddef>
#include <netinet/in.h>

namespace rso {

struct Message;

template <typename T>
T swap_endian(const T& value) {
  // check if big endian
  if(htons(10) == 10) {
    return value;
  }
  T result;
  char* from = (char*) &value;
  char* to = (char*) &result;

  for(std::size_t i = 0; i < sizeof(value); i++) {
    to[i] = from[sizeof(value) - i - 1];
  }
  return result;
}

void send_all(int sock_fd, const void* buffer, std::size_t length);
void receive_all(int sock_fd, void* buffer, std::size_t length);
void sendMsg(int sock_fd, const Message& msg);
Message receiveMsg(int sock_fd);

}
#endif