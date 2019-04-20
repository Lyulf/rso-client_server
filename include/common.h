#ifndef RSO_COMMON_H
#define RSO_COMMON_H

#include <cstddef>
#include <netinet/in.h>

namespace rso {

struct Message;

template <typename T>
T swap_endian(const T value) {
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

void send_all(int sock_fd, void* buffer, size_t length);
void receive_all(int sock_fd, void* buffer, size_t length);
void msg_hton(Message* msg);
void msg_ntoh(Message* msg);

}
#endif