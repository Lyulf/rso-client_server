#ifndef RSO_MESSAGE_TYPES_H
#define RSO_MESSAGE_TYPES_H

#include <cstdint>
#include <array>

namespace rso {
namespace msg_type {

constexpr uint8_t request = 0;
constexpr uint8_t response = 1;

constexpr uint8_t sqrt = 0;
constexpr uint8_t date = 1;

constexpr std::array<uint8_t, 4> sqrt_request {request, 0, 0, sqrt};
constexpr std::array<uint8_t, 4> sqrt_response {response, 0, 0, sqrt};

constexpr std::array<uint8_t, 4> date_request {request, 0, 0, date};
constexpr std::array<uint8_t, 4> date_response {response, 0, 0, date};

}
}

#endif