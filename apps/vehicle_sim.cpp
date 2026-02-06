#include "../net/udp_link.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>
#include <mutex>

std::atomic<bool> vehicle_stopped(false);
UDPLink* global_udp_link = nullptr; // global pointer for now so signal handler can access it

class Vehicle {
    public:
    // Default constructor
    Vehicle(UDPLink* link) {
        xpos = 0.0;
        ypos = 0.0;
        x_velocity = 5.0; // fixed for now
        y_velocity = 1.0; // fixed for now
        heading = 0; // 0/360 degrees = North
        battery = 100;

        udp_link = link;

    }
    
    void print_info() {
        std::cout << "POS: (" << xpos << "," << ypos << ")" << std::endl;
        std::cout << "Horizontal Velocity: " << x_velocity << "mph" << std::endl;
        std::cout << "Vertical Velocity: " << y_velocity << "mph" << std::endl;
        std::cout << "Heading: " << heading << "deg" << std::endl;
        std::cout << "Bat: " << battery << "%" << std::endl;
    }

    void start_vehicle() {
        vehicle_thread = std::thread(&Vehicle::begin_vehicle_sim, this);
        udp_publishing_thread = std::thread(&Vehicle::start_publishing_thread, this);
    }

    void stop_vehicle() {
        if (vehicle_thread.joinable()) {
            vehicle_thread.join();
        }
        if (udp_publishing_thread.joinable()) {
            udp_publishing_thread.join();
        }
    }
    
    private:
    float xpos; // meters
    float ypos; // meters
    float x_velocity; // meters / second (fixed velocity for now)
    float y_velocity;
    int heading;
    int battery;

    std::thread vehicle_thread;
    std::thread udp_publishing_thread;
    std::mutex data_mutex; // mutex to protect shared access to vehicle state

    UDPLink *udp_link;


    void begin_vehicle_sim() {
        int battery_counter = 0;
        while (!vehicle_stopped) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // bandaid method to adjust variables at for now, will add physics later
            float target_xpos = 500.0;
            float target_ypos = 750.0;

            // update vehicle state
            std::lock_guard<std::mutex> lock(data_mutex); // lock data access
            if (xpos < target_xpos) {
                xpos += x_velocity / 10; // velocity / 10 because our vehicle updates on a 100 ms cycle
            } else {
                xpos -= x_velocity / 10;
            }
            
            if (ypos < target_ypos) {
                ypos += y_velocity / 10;
            } else {
                ypos -= y_velocity / 10;
            }

            if (battery_counter > 100) {
                battery -= 1;
                battery_counter = 0;
            }
            battery_counter += 1;

        }

        std::cout << "vehicle stopping..." << std::endl;
    }

    void start_publishing_thread() {
        while (!vehicle_stopped) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // publish vehicle state every second

            float current_xpos, current_ypos;
            int current_battery;
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                current_xpos = xpos;
                current_ypos = ypos;
                current_battery = battery;
            }

            std::string message = "POS: (" + std::to_string(current_xpos) + ", " + std::to_string(current_ypos) + "), " + "Battery: " + std::to_string(current_battery) + "%";

            udp_link->send_message(message, "127.0.0.1", 12346);
        }
    }
    
};

void signal_handler(int signum) {
    std::cout << "Interrupt signal" << std::endl;
    vehicle_stopped = true;
}

int main() {
    std::signal(SIGINT, signal_handler);

    std::cout << "Vehicle sim" << std::endl;

    UDPLink udp_link(12345);
    global_udp_link = &udp_link;

    Vehicle veh(&udp_link);
    udp_link.start_receive_thread();
    veh.start_vehicle();

    while (!vehicle_stopped) {
        // print vehicle info every 2 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        veh.print_info();
        std::cout << std::endl;
    }


    veh.stop_vehicle();
    udp_link.stop_receive_thread();

    std::cout << "Exiting program..." << std::endl;

    return 0;
}