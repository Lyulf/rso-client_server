#include "../include/common.h"
#include "../include/defaults.h"
#include "../include/message.h"
#include "../include/server.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <math.h>
#include <mutex>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace rso;

struct Server::ServerImpl {
  ServerImpl(const short port, const size_t max_clients, const int max_queue)
    : domain(defaults::domain), port(port), max_clients(max_clients),
      no_clients(0), max_queue(max_queue), mutex() {
    if(max_clients < static_cast<unsigned int>(max_queue)) {
      throw std::invalid_argument("max_clients can't be smaller than max_queue");
    }
    initAddress();
    initServer();
    std::cout << "Server initialized" << std::endl;
  }
  
  ~ServerImpl() {

  }

  void initAddress() {
    address.sin_family = domain;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    address_length = sizeof(address);
  }

  void initServer() {
    master_socket = socket(domain, SOCK_STREAM, 0);
    if(master_socket == -1) {
      throw std::runtime_error("Failed to create socket.\nErrno: " + std::string(strerror(errno)));
    }
    int optval = 1;
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
      throw std::runtime_error("Cannot set SO_REUSEADDR.\nErrno: " + std::string(strerror(errno)));
    }
    if(bind(master_socket, (sockaddr*) &address, address_length) == -1) {
      throw std::runtime_error("Failed to bind the address.\nErrno: " + std::string(strerror(errno)));
    }
    if(listen(master_socket, max_queue) == -1) {
      throw std::runtime_error("Failed to prepare to accept connections.\nErrno: " + std::string(strerror(errno)));
    }
  }

  void acceptClients() {
    int client_socket;
    socklen_t client_length;
    sockaddr_in client_address;
    client_length = sizeof(client_address);
    std::cout << "Accepting clients" << std::endl;
    while(true) {
      mutex.lock();
      if(no_clients + 1 < max_clients) {
        mutex.unlock();
        try {
          client_socket = accept(master_socket, (sockaddr*) &client_address, &client_length);
          if(client_socket == -1) {
            throw std::runtime_error("Failed to accept client.\nErrno: " + std::string(strerror(errno)));
          }
          std::cout << "Client " << client_socket << " connected" << std::endl;
          mutex.lock();
          no_clients++;
          mutex.unlock();
          std::thread(&ServerImpl::handleClient, this, (void*) &client_socket).detach();
        } catch(std::runtime_error& e) {
          std::cout << e.what() << std::endl;
        }
      } else {
        mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
    }
  }

  void handleClient(void* client) {
    int client_socket = *(int*) client;

    try {
      decltype(Message::type) type;
      while(true) {
        receive_all(client_socket, &type, sizeof(type));
        switch(type[3]) {
        case 1:
          handleSqrtRequest(client_socket);
          break;
        case 2:
          handleDateRequest(client_socket);
          break;
        default:
          std::cout << "Unknown request '" << type << "' from " << client_socket << std::endl;
          break;
        }
      }
    } catch(std::runtime_error& e) {
      std::cout << e.what() << std::endl;
    }
    std::cout << "Closing client " << client_socket << std::endl;
    close(client_socket);
    mutex.lock();
    no_clients--;
    mutex.unlock();
  }

  void handleSqrtRequest(int client_socket) {
    SqrtMsg msg;
    msg.setType(1, 0, 0, 1);
    receive_all(client_socket, &msg.id, sizeof(msg) - offsetof(Message, id));
    msg_ntoh(&msg);
    std::cout << "Recieved sqrt request from " << client_socket << " value = " << msg.value << std::endl;
    msg.value = sqrt(msg.value);
    msg_hton(&msg);
    send_all(client_socket, &msg, sizeof(msg));
    std::cout << "Result " << swap_endian(msg.value) << " sent to " << client_socket << std::endl;
  }

  void handleDateRequest(int client_socket) {
    DateMsg msg;
    msg.setType(1, 0, 0, 2);
    receive_all(client_socket, &msg + offsetof(DateMsg, id), sizeof(msg.id));
    msg_ntoh(&msg);
    std::cout << "Recieved date request from " << client_socket << std::endl;
    auto time = std::chrono::system_clock::now();
    auto time_struct = std::chrono::system_clock::to_time_t(time);
    msg.date = ctime(&time_struct);
    msg.length = msg.date.size();
    msg_hton(&msg);
    send_all(client_socket, &msg, offsetof(DateMsg, date));
    send_all(client_socket, msg.date.data(), msg.date.size());
    std::cout << "Date sent to " << client_socket << std::endl;
  }

  const int domain, port;
  size_t max_clients, no_clients;
  int max_queue, master_socket;
  sockaddr_in address;
  socklen_t address_length;
  std::mutex mutex;
};

Server::Server(const short port, const size_t max_clients, const int max_queue)
  : impl(std::make_unique<ServerImpl>(port, max_clients, max_queue)) { }

Server::~Server() = default;

void Server::acceptClients() {
  impl->acceptClients();
}