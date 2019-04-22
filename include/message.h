#ifndef RSO_MESSAGE_H
#define RSO_MESSAGE_H

#include <cstdint>
#include <string>
#include <iostream>
#include <array>

namespace rso {

struct Message {
  std::array<std::uint8_t, 4> type;
  std::uint32_t id;
  double value;
  std::string date;
};

template <std::size_t SIZE>
std::ostream& operator<<(std::ostream& os, const std::array<std::uint8_t, SIZE>& array) {
  for(std::size_t i = 0; i < SIZE; i++) {
    os << static_cast<short>(array[i]);
  }
  return os;
}

}

#endif