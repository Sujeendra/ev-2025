#include <linux/can.h>
#include <linux/can/j1939.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <errno.h>

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <dbcppp/CApi.h>
#include <fstream>
#include <unordered_map>
#include "dbcppp/Network.h"
#include <optional>
#include <mutex>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

const char *ifname = "can0";

// Function to send a CAN J1939 request message
void send_request() {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        std::cerr << "Error creating socket in send_request thread" << std::endl;
        return;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex(ifname);
    if (addr.can_ifindex == 0) {
        std::cerr << "Error finding CAN interface in send_request thread" << std::endl;
        close(sock);
        return;
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding socket in send_request thread" << std::endl;
        close(sock);
        return;
    }

    struct can_frame frame;
    std::memset(&frame, 0, sizeof(frame));

    // Set up the Request PGN (0xEA00)
    frame.can_id = 0x18EA0000 | CAN_EFF_FLAG;   // Priority 6, PGN 0xEA00 (Request), destination address 0x01
    frame.can_dlc = 3;           // DLC = 3 bytes for requested PGN

    // Specify the PGN you are requesting (Time/Date PGN 0x0FEE6)
    frame.data[0] = 0xE6;        // Low byte of the requested PGN (0x0FEE6)
    frame.data[1] = 0xFE;        // Middle byte of the requested PGN
    frame.data[2] = 0x0F;        // High byte of the requested PGN

    // Loop to send the request message every 1 second
    while (true) {
        int bytes_sent = send(sock, &frame, sizeof(frame), 0);
        if (bytes_sent < 0) {
            std::cerr << "Error sending message in send_request thread" << std::endl;
            std::cerr << "Error code: " << errno << std::endl;
        } else {
            std::cout << "Sent J1939 Request PGN 0xEA00 for PGN 0x0FEE6 to address 0x01" << std::endl;
        }

        // Wait for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(sock);  // Close the socket when done
}

// Function to read messages from the CAN interface
void read_responses() {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        std::cerr << "Error creating socket in read_responses thread" << std::endl;
        return;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex(ifname);
    if (addr.can_ifindex == 0) {
        std::cerr << "Error finding CAN interface in read_responses thread" << std::endl;
        close(sock);
        return;
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding socket in read_responses thread" << std::endl;
        close(sock);
        return;
    }

    struct can_frame read_frame;

    while (true) {
        int nbytes = read(sock, &read_frame, sizeof(read_frame));
        if (nbytes > 0) {
            // Check if PGN 61427 is received (0x0CEFF321)
            if ((read_frame.can_id & 0x1FFFFFFF) == 0x0CEFF321) {
                if (((read_frame.data[0] & 0x1C) >> 2) == 0x3) {
                    std::cout << "Received Connect indicating 'Requested'" << std::endl;
                    // Form frame and send additional data if needed
                    struct can_frame frame;
                    std::memset(&frame, 0, sizeof(frame));

                    // Set up the Request PGN (0xEA00)
                    frame.can_id = 0x14F096F3 | CAN_EFF_FLAG;   
                    frame.can_dlc = 8;    

                    frame.data[0] = 0x00;        
                    frame.data[1] = 0x00;        
                    frame.data[2] = 0x00;   
                    frame.data[3] = 0x00;        
                    frame.data[4] = 0x00;        
                    frame.data[5] = 0x01; 
                    frame.data[6] = 0x00;        
                    frame.data[7] = 0x00;        
                    send(sock, &frame, sizeof(frame), 0);

                }
                // Handle PGN 61590 messages or disconnections as needed
                else if (((read_frame.data[0] & 0x1C) >> 2) == 0x0) {
                    std::cout << "Received Disconnect indicating 'Requested'" << std::endl;
                    // Form frame and send additional data if needed
                    struct can_frame frame;
                    std::memset(&frame, 0, sizeof(frame));

                    // Set up the Request PGN (0xEA00)
                    frame.can_id = 0x14F096F3 | CAN_EFF_FLAG;   
                    frame.can_dlc = 8;    

                    frame.data[0] = 0x00;        
                    frame.data[1] = 0x00;        
                    frame.data[2] = 0x00;   
                    frame.data[3] = 0x00;        
                    frame.data[4] = 0x00;        
                    frame.data[5] = 0x04; 
                    frame.data[6] = 0x00;        
                    frame.data[7] = 0x00;        
                    send(sock, &frame, sizeof(frame), 0);
                }
            }

        } else if (nbytes < 0) {
            std::cerr << "Error reading message in read_responses thread" << std::endl;
        }
    }

    close(sock);  // Close the socket when done
}

int main() {
    // Create separate threads for sending and receiving CAN messages
    std::thread sender(send_request);
    std::thread receiver(read_responses);

    // Join threads to main thread
    sender.join();
    receiver.join();

    return 0;
}
