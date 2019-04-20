#include "../include/client.h"
#include "../include/defaults.h"
#include <iostream>
#include <limits.h>
#include <string.h>

using namespace rso;

int main() {
  try {
    Client client(defaults::address);
    std::shared_ptr<Message> msg;
    decltype(Message::type) request_type = {0, 0, 0, 0};
    std::cout << "Types: " << std::endl;
    std::cout << "1: Sqrt Request" << std::endl;
    std::cout << "2: Date Request\n" << std::endl;
    while(true) {
      if(std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      std::cout << "Enter connection type: ";
      std::cin >> request_type[3];
      request_type[3] -= '0';
      float value = 0.0;
      bool invalid_request = false;
      switch(request_type[3]) {
      case 1:
        std::cout << "Enter value: ";
        std::cin >> value;
      case 2:
        client.sendRequest(request_type, value);
        break;
      default:
        invalid_request = true;
        break;
      }
      if(invalid_request) {
        std::cout << "Invalid request type: " << request_type[0] << request_type[1] << request_type[2] << request_type[3] << std::endl;
        continue;
      }
      msg = client.receiveRequest();
      std::cout << "\ntype: " << msg->type[0] << msg->type[1] << msg->type[2] << msg->type[3] << std::endl;
      std::cout <<  "id: " << msg->id << std::endl;
      switch(msg->type[3]) {
      case 1:
        std::cout << "result: " << std::static_pointer_cast<SqrtMsg>(msg)->value << std::endl;
        break;
      case 2:
        std::cout << "length: " << std::static_pointer_cast<DateMsg>(msg)->length << std::endl;
        std::cout << "date: " << std::static_pointer_cast<DateMsg>(msg)->date << std::endl;
      }
    }
  } catch(std::runtime_error& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}