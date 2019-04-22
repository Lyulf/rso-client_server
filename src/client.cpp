#include "../include/common.h"
#include "../include/client.h"
#include "../include/defaults.h"
#include "../include/message_types.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>

using namespace rso;

class Client::ClientImpl {
public:
  ClientImpl(const std::string& ipv4, const short port)
    : port(port), noRequests(1) {
      initAddress(ipv4);
      client_socket = socket(defaults::domain, SOCK_STREAM, 0);
      if(client_socket == -1) {
        throwErrno<std::runtime_error>("Failed to create a socket.");
      }
      if(connect(client_socket, (sockaddr*) &address, address_length) == -1) {
        throwErrno<std::runtime_error>("Failed to connect to server.");
      }
      std::cout << "Successfully conected" << std::endl;
  }

  ~ClientImpl() {

  }

  void initAddress(std::string ipv4) {
      address.sin_family = defaults::domain;
      address.sin_addr.s_addr = inet_addr(ipv4.c_str());
      address.sin_port = htons(port);
      address_length = sizeof(address);
  }

  void sendSqrtRequest(double value) {
    Message msg;
    msg.type = msg_type::sqrt_request;
    msg.id = noRequests++;
    msg.value = value;
    sendMsg(client_socket, msg);
    std::cout << "Sqrt request sent" << std::endl;
  }

  void sendDateRequest() {
    Message msg;
    msg.type = msg_type::date_request;
    msg.id = noRequests++;
    sendMsg(client_socket, msg);
    std::cout << "Date request sent" << std::endl;
  }

  Message receiveRequest() {
    Message msg = receiveMsg(client_socket);
    return msg;
  }
  
  int client_socket;
  const short port;
  sockaddr_in address;
  socklen_t address_length;
  uint32_t noRequests;
};

Client::Client(const std::string& ipv4, const short port)
  : impl(std::make_unique<ClientImpl>(ipv4, port)) {
}

Client::~Client() {

}

void Client::sendSqrtRequest(double value) {
  impl->sendSqrtRequest(value);
}

void Client::sendDateRequest() {
  impl->sendDateRequest();
}

Message Client::receiveRequest() {
  return impl->receiveRequest();
}