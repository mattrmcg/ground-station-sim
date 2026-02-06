#include "../net/udp_link.hpp"
#include <iostream>
#include <csignal>
#include <string>

std::atomic<bool> ground_station_active(false);

void signal_handler(int signum) {
    std::cout << "Interrupt signal" << std::endl;
    ground_station_active = false;
}

int main() {
    std::signal(SIGINT, signal_handler);

    std::cout << "Ground station" << std::endl;

    UDPLink udp_link(12346);
    udp_link.start_receive_thread();

    ground_station_active = true;

    while (ground_station_active) {
    }

    udp_link.stop_receive_thread();
    std::cout << "Exiting program..." << std::endl;
    
    return 0;
}