#include "../include/common.h"
#include "../include/defaults.h"
#include "../include/disconnected.h"
#include "../include/message.h"
#include "../include/message_types.h"
#include "../include/server.h"
#include <arpa/inet.h>
#include <chrono>
#include <condition_variable>
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
      no_clients(0), max_queue(max_queue), mutex(), cond_var() {
    initAddress();
    initSocket();
    setOpt();
    bindAddress();
    listenOnMaster();
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

  void initSocket() {
    master_socket = socket(domain, SOCK_STREAM, 0);
    if(master_socket == -1) {
      throw std::runtime_error("Failed to create socket.\nErrno: " + std::string(strerror(errno)));
    }
  }

  void setOpt() {
    int optval = 1;
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
      throw std::runtime_error("Cannot set SO_REUSEADDR.\nErrno: " + std::string(strerror(errno)));
    }
  }

  void bindAddress() {
    if(bind(master_socket, (sockaddr*) &address, address_length) == -1) {
      throw std::runtime_error("Failed to bind the address.\nErrno: " + std::string(strerror(errno)));
    }
  }

  void listenOnMaster() {
    if(listen(master_socket, max_queue) == -1) {
      throw std::runtime_error("Failed to prepare to accept connections.\nErrno: " + std::string(strerror(errno)));
    }
  }

  void acceptClients() {
    int client_socket;
    std::unique_ptr<sockaddr_in> client_address;
    std::unique_ptr<socklen_t> client_length;
    std::cout << "Accepting clients" << std::endl;
    while(true) {
      client_address = std::make_unique<sockaddr_in>();
      client_length = std::make_unique<socklen_t>(sizeof(sockaddr_in));
      {
        std::unique_lock<std::mutex> lock(mutex);
        cond_var.wait(lock, [this] {
          std::cout << "left=" << no_clients + 1 << " right=" << max_clients << std::endl;
          return no_clients < max_clients;}
          );
      }
      try {
        client_socket = accept(master_socket, (sockaddr*) client_address.get(), client_length.get());
        if(client_socket == -1) {
          throw std::runtime_error("Failed to accept client.\nErrno: " + std::string(strerror(errno)));
        }
        {
          std::lock_guard<std::mutex> lock(mutex);
          no_clients++;
        }
        std::cout << "Client " << client_socket << " connected" << std::endl;
        std::thread(&ServerImpl::handleClient, this, client_socket, std::move(client_address), std::move(client_length)).detach();
      } catch(std::runtime_error& e) {
        std::cout << e.what() << std::endl;
      }
    }
  }

  void handleClient(
    int client_socket,
    std::unique_ptr<sockaddr_in> client_address,
    std::unique_ptr<socklen_t> client_length
    ) {
      UNUSED(client_length);
      UNUSED(client_address);
      try {
        while(true) {
          Message msg = receiveMsg(client_socket);
          if(msg.type == msg_type::sqrt_request) {
            handleSqrtRequest(client_socket, msg.id, msg.value);
          } else if(msg.type == msg_type::date_request) {
            handleDateRequest(client_socket, msg.id);
          } else {
            std::cout << "Unknown request '" << msg.type << "' from " << client_socket << std::endl;
          }
        }
      } catch(DisconnectedException& e) {
        std::cout << "Client " << client_socket << " disconnected" << std::endl;
      } catch(std::runtime_error& e) {
        std::cout << e.what() << std::endl;
      }
      close(client_socket);
      {
        std::lock_guard<std::mutex> lock(mutex);
        no_clients--;
      }
      cond_var.notify_all();
  }

  void handleSqrtRequest(int client_socket, uint32_t id, double value) {
    std::cout << "Recieved sqrt request from " << client_socket << " value = " << value << std::endl;
    Message msg;
    msg.type = msg_type::sqrt_response;
    msg.id = id;
    msg.value = sqrt(value);
    sendMsg(client_socket, msg);
    std::cout << "Result " << msg.value << " sent to " << client_socket << std::endl;
  }

  void handleDateRequest(int client_socket, uint32_t id) {
    auto time = std::chrono::system_clock::now();
    auto time_struct = std::chrono::system_clock::to_time_t(time);
    std::cout << "Recieved date request from " << client_socket << std::endl;
    Message msg;
    msg.type = msg_type::date_response;
    msg.id = id;
    msg.date = ctime(&time_struct);
    sendMsg(client_socket, msg);
    std::cout << "Date sent to " << client_socket << std::endl;
  }

  const int domain, port;
  std::size_t max_clients;
  std::size_t no_clients;
  int max_queue, master_socket;
  sockaddr_in address;
  socklen_t address_length;
  std::mutex mutex;
  std::condition_variable cond_var;
};

Server::Server(const short port, const size_t max_clients, const int max_queue)
  : impl(std::make_unique<ServerImpl>(port, max_clients, max_queue)) { }

Server::~Server() = default;

void Server::acceptClients() {
  impl->acceptClients();
}