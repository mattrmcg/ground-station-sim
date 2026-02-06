#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <optional>

struct VehicleState {
    int seq;
    float xpos;
    float ypos;
    float speed;
    float heading;
    float battery;
};

class Protocol {
    public:
    // encode a VehicleState into a protocol string
    static std::string encode(const VehicleState& state);

    // decode a protocol string into a VehicleState
    static std::optional<VehicleState> decode(const std::string& message);
};

#endif