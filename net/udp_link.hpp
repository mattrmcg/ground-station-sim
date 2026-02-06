#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <thread>

#ifndef UDPLINK_HPP
#define UDPLINK_HPP

class UDPLink {
    public:
        UDPLink(int port); // constructor
        ~UDPLink(); // destructor for cleaning up resources

        void send_message(const std::string& message, const std::string& ip, int port);
        std::string receive_message();

        void start_receive_thread(); // start thread to receive messages
        void stop_receive_thread(); // stop receive thread


    private:
        int udp_socket; // udp socket file descriptor
        struct sockaddr_in server_addr; // address to bind to
        struct sockaddr_in client_addr; // address for sending/receiving messages

        std::thread receive_thread; // receive thread object
        std::atomic<bool> receive_thread_running;
};

#endif