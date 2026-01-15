// Networking
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// System
#include <unistd.h>
#include <cstring>
#include <cerrno>

// C++ standard library
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#define PORT 8080

// Reader Thread calls recv() and prints the message from the server
void reader_loop(int sock) {
    char buffer[1024] = {0};
    while (true) {
        int bytes_received = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << "Echo from server: " << buffer << "\n";
        std::cout.flush();
    }
}

// Writer Thread to send inputs to server
int main() {
    struct sockaddr_in serv_addr;

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 to IPv6 addresses from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        return -1;
    }

    std::thread(reader_loop, sock).detach(); // thread reader_loop

    std::string client_msg;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, client_msg);
        if (client_msg.empty()) {
            continue;
        }
        send(sock, client_msg.c_str(), client_msg.size(), 0);
    }

    // Close socket
    close(sock);
    return 0;
}


/*
Compile files
g++ -std=c++17 -pthread -o server server.cpp
g++ -std=c++17 -pthread -o client client.cpp

Run server
./server

Run client in another terminal
./client
*/

// Step 1: Separate client and server responsibilities
// Server receives message from one client, server sends to other clients

// Step 2: Define a message format (protocol)

// Step 3: Handle multiple clients

// Step 4: Handle disconnections