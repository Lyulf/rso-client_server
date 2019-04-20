#include "../include/common.h"
#include "../include/client.h"
#include "../include/defaults.h"
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
        throw std::runtime_error("Failed to create a socket.\nErrno: " + std::string(strerror(errno)));
      }
      if(connect(client_socket, (sockaddr*) &address, address_length) == -1) {
        throw std::runtime_error("Failed to connect to server.\nErrno: " + std::string(strerror(errno)));
      }
      std::cout << "Successfully conected" << std::endl;
  }

  ~ClientImpl() {

  }

  void initAddress(const std::string ipv4) {
      address.sin_family = defaults::domain;
      address.sin_addr.s_addr = inet_addr(ipv4.c_str());
      address.sin_port = htons(port);
      address_length = sizeof(address);
  }

  void sendRequest(const decltype(Message::type) type, const double value) {
    switch(type[3]) {
    case 1:
      sendSqrtRequest(value);
      break;
    case 2:
      sendDateRequest();
      break;
    }
  }

  void sendSqrtRequest(const double value) {
    SqrtMsg msg;
    msg.setType(0, 0, 0, 1);
    msg.id = noRequests++;
    msg.value = value;
    msg_hton(&msg);
    send_all(client_socket, &msg, sizeof(msg));
    std::cout << "Sqrt request sent" << std::endl;
  }

  void sendDateRequest() {
    Message msg;
    msg.setType(0, 0, 0, 2);
    msg.id = noRequests++;
    msg_hton(&msg);
    send_all(client_socket, &msg, sizeof(msg));
    std::cout << "Date request sent" << std::endl;
  }

  std::shared_ptr<Message> receiveRequest() {
    decltype(Message::type) type;
    receive_all(client_socket, type, offsetof(Message, id));
    if(type[3] == 1) {
      SqrtMsg msg;
      msg.setType(type);
      receive_all(client_socket, &msg.id, sizeof(SqrtMsg) - offsetof(SqrtMsg, id));
      std::cout << "Sqrt request received" << std::endl;
      msg_ntoh(&msg);
      return std::make_shared<SqrtMsg>(msg);
    } else if(type[3] == 2) {
      DateMsg msg;
      msg.setType(type);
      receive_all(client_socket, &msg.id, offsetof(DateMsg, date) - offsetof(DateMsg, id));
      msg.date.resize(ntohl(msg.length)); // setting size manualy otherwise it will be 0
      receive_all(client_socket, &msg.date.front(), ntohl(msg.length)); // probably undefined behavior
      std::cout << "Date request received" << std::endl;
      msg_ntoh(&msg);
      return std::make_shared<DateMsg>(msg);
      return nullptr;
    } else {
      std::cout << "Unknown request of type " << type[0] << type[1] << type[2] << type[3] <<  " received" << std::endl;
      return nullptr;
    }
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

void Client::sendRequest(const decltype(Message::type) type, const double value) {
  impl->sendRequest(type, value);
}

std::shared_ptr<Message> Client::receiveRequest() {
  return impl->receiveRequest();
}