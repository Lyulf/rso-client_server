#ifndef RSO_MESSAGE_H
#define RSO_MESSAGE_H

#include <cstdint>
#include <string>
#include <iostream>

namespace rso {

#pragma pack(push, 1)
struct Message {
  uint8_t type[4];
  uint32_t id;

  Message() : type {0, 0, 0, 0} { }
  Message(const Message& other) 
    : Message() {
    setType(other.type);
    id = other.id;
  }

  inline void setType(uint8_t i, uint8_t j, uint8_t k, uint8_t l) {
    type[0] = i;
    type[1] = j;
    type[2] = k;
    type[3] = l;
  }
  inline void setType(const decltype(type)& other) {
    type[0] = other[0];
    type[1] = other[1];
    type[2] = other[2];
    type[3] = other[3];
  }
};

struct SqrtMsg : public Message {
  float value;
};

struct DateMsg : public Message {
  uint32_t length;
  std::string date;
};
#pragma pack(pop)

}

#endif