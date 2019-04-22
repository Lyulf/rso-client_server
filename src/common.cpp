#include "../include/common.h"
#include "../include/disconnected.h"
#include "../include/message.h"
#include "../include/message_types.h"
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

namespace rso {

void send_all(int sock_fd, const void* buffer, std::size_t length) {
  char* ptr = (char*) buffer;
  while(length > 0) {
    int i = send(sock_fd, ptr, length, 0);
    if(i == 0) {
      throw DisconnectedException("Lost connection");
    } else if(i == -1) {
      if(errno == EINTR) {
        std::cout << "Error EINTR occured. Continuing." << std::endl;
        continue;
      }
      throw std::runtime_error("Error while sending data.\nErrno: " + std::string(strerror(errno)));
    }
    ptr += i;
    length -= i;
  }
}

void receive_all(int sock_fd, void* buffer, std::size_t length) {
  char* ptr = (char*) buffer;
  while(length > 0) {
    int i = recv(sock_fd, ptr, length, 0);
    if(i == 0) {
      throw DisconnectedException("Lost connection");
    } else if(i == -1) {
      if(errno == EINTR) {
        std::cout << "Error EINTR occured. Continuing." << std::endl;
        continue;
      }
      throw std::runtime_error("Error while receiving data.\nErrno: " + std::string(strerror(errno)));
    }
    ptr += i;
    length -= i;
  }
}


void sendMsg(int sock_fd, const Message& msg) {
  send_all(sock_fd, msg.type.data(), msg.type.size());
  auto id = swap_endian(msg.id);
  send_all(sock_fd, &id, sizeof(msg.id));
  if(msg.type[3] == msg_type::sqrt) {
    auto value = swap_endian(msg.value);
    send_all(sock_fd, &value, sizeof(value));
  } else if(msg.type == msg_type::date_response) {
    auto size = msg.date.size();
    size = swap_endian(size);
    send_all(sock_fd, &size, sizeof(size));
    send_all(sock_fd, msg.date.data(), msg.date.size());
  }
}

Message receiveMsg(int sock_fd) {
  Message msg;
  receive_all(sock_fd, msg.type.data(), msg.type.size());
  receive_all(sock_fd, &msg.id, sizeof(msg.id));
  msg.id = swap_endian(msg.id);
  if(msg.type[3] == msg_type::sqrt) {
    receive_all(sock_fd, &msg.value, sizeof(msg.value));
    msg.value = swap_endian(msg.value);
  } else if(msg.type == msg_type::date_response) {
    std::size_t size;
    receive_all(sock_fd, &size, sizeof(size));
    size = swap_endian(size);
    msg.date.resize(size);
    receive_all(sock_fd, msg.date.data(), size);
  }
  return msg;
}

}