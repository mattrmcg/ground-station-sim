#include "udp_link.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

// constructor
UDPLink::UDPLink(int port) {
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // bind to all available interfaces
    server_addr.sin_port = htons(port); 

    if (bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    receive_thread_running = false;
}

// destructor
UDPLink::~UDPLink() {
    stop_receive_thread();
    close(udp_socket);
}

// send message to specific ip and port
void UDPLink::send_message(const std::string& message, const std::string& ip, int port) {
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);

    // convert ip address from string "presentation" to "network" byte order
    inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr);

    sendto(udp_socket, message.c_str(), message.size(), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
}

// receive a single message
std::string UDPLink::receive_message() {
    char buffer[1024];
    socklen_t addr_len = sizeof(client_addr);

    // file descriptor set for select
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(udp_socket, &read_fds);

    // set timeout for select()
    struct timeval timeout;
    timeout.tv_sec = 1; // 1 second timeout
    timeout.tv_usec = 0;

    // wait for data to be available from socket
    int ret = select(udp_socket + 1, &read_fds, nullptr, nullptr, &timeout);
    if (ret > 0 && FD_ISSET(udp_socket, &read_fds)) {
        // Data is available, call recvfrom()
        ssize_t len = recvfrom(udp_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

        if (len > 0) {
            buffer[len] = '\0';
            return std::string(buffer);
        }
    } else if (ret == 0) {
        // Timeout occured, no data received
        return "";
    } else {
        perror("Error in select()");
    }
    return "";
}

// start thread to continuously receive messages
void UDPLink::start_receive_thread() {
    receive_thread_running = true;
    receive_thread = std::thread([this]() {
        while (receive_thread_running) {
            std::string message = receive_message();
            if (!message.empty()) {
                std::cout << "Received: " << message << std::endl;
            }
        }
    });
}

// stop the receive thread
void UDPLink::stop_receive_thread() {
    if (receive_thread_running) {
        receive_thread_running = false;
        if (receive_thread.joinable()) {
            receive_thread.join();
        }
    }
}