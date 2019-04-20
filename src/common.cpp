#include "../include/common.h"
#include "../include/disconnected.h"
#include "../include/message.h"
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

namespace rso {

void send_all(int sock_fd, void* buffer, size_t length) {
  char* ptr = (char*) buffer;
  while(length > 0) {
    int i = send(sock_fd, ptr, length, 0);
    if(i == 0) {
      throw std::runtime_error("Server is offline");
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

void receive_all(int sock_fd, void* buffer, size_t length) {
  char* ptr = (char*) buffer;
  while(length > 0) {
    int i = recv(sock_fd, ptr, length, 0);
    if(i == 0) {
      throw DisconnectedException("Client disconnected");
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

void msg_hton(Message* msg) {
  msg->id = htonl(msg->id);
  if(msg->type[3] == 1) {
    SqrtMsg* temp = static_cast<SqrtMsg*>(msg);
    temp->value = swap_endian(temp->value);
  } else if(msg->type[0] == 1 && msg->type[3] == 2) {
    DateMsg* temp = static_cast<DateMsg*>(msg);
    temp->length = htonl(temp->length);
  }
}

void msg_ntoh(Message* msg) {
  msg->id = ntohl(msg->id);
  if(msg->type[3] == 1) {
    SqrtMsg* temp = static_cast<SqrtMsg*>(msg);
    temp->value = swap_endian(temp->value);
  } else if(msg->type[0] == 1 && msg->type[3] == 2) {
    DateMsg* temp = static_cast<DateMsg*>(msg);
    temp->length = ntohl(temp->length);
  }
}

}