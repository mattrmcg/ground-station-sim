#include "protocol.hpp"
#include <sstream>
#include <iomanip>
#include <regex>

std::string Protocol::encode(const VehicleState& state) {
    std::ostringstream oss;
    oss << "SEQ=" << state.seq
        << " X=" << std::fixed << std::setprecision(2) << state.xpos
        << " Y=" << std::fixed << std::setprecision(2) << state.ypos
        << " SPD=" << std::fixed << std::setprecision(2) << state.speed
        << " HDG=" << std::fixed << std::setprecision(2) << state.heading
        << " BAT=" << std::fixed << std::setprecision(2) << state.battery;
    return oss.str();
}

// decode a protocol string into a VehicleState
std::optional<VehicleState> Protocol::decode(const std::string& message) {
    std::regex regex(R"(SEQ=(\d+)\s+X=([\d.]+)\s+Y=([\d.]+)\s+SPD=([\d.]+)\s+HDG=([\d.]+)\s+BAT=([\d.]+))");
    std::smatch match;

    if (std::regex_match(message, match, regex)) {
        VehicleState state:
        state.seq = std::stoi(match[1]);
        state.xpos = std::stoi(match[2]);
        state.ypos = std::stoi(match[3]);
        state.speed = std::stof(match[4]);
        state.heading = std::stof(match[5]);
        state.battery = std::stof(match[6]);

        if (validate(state)) {
            return state;
        }
    }

    return std::nullopt; // return empty optional if decoding fails
}
