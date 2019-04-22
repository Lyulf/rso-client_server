#include "../include/client.h"
#include "../include/defaults.h"
#include "../include/disconnected.h"
#include "../include/message_types.h"
#include <iostream>
#include <limits.h>
#include <string.h>

using namespace rso;

int main() {
  try {
    Client client(defaults::address);
    int type;
    std::cout << "Types: " << std::endl;
    std::cout << "1: Sqrt Request" << std::endl;
    std::cout << "2: Date Request\n" << std::endl;
    while(true) {
      if(std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      std::cout << "Enter connection type: ";
      std::cin >> type;
      double value;
      bool invalid_request = false;
      switch(type) {
      case 1:
        std::cout << "Enter value: ";
        std::cin >> value;
        client.sendSqrtRequest(value);
        break;
      case 2:
        client.sendDateRequest();
        break;
      default:
        invalid_request = true;
        break;
      }
      if(invalid_request) {
        std::cout << "Invalid request type: " << type << std::endl;
        continue;
      }
      Message msg = client.receiveRequest();
      std::cout << "\ntype: " << msg.type << std::endl;
      std::cout <<  "id: " << msg.id << std::endl;
      if(msg.type == msg_type::sqrt_response) {
        std::cout << "result: " << msg.value << std::endl;
      } else if(msg.type == msg_type::date_response) {
        std::cout << "length: " << msg.date.size() << std::endl;
        std::cout << "date: " << msg.date << std::endl;
      }
    }
  } catch(std::runtime_error& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}