#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char *hello = "Hello from client";
    const char *message_two = "This is my second message";

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
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

    // Send data
    send(sock, hello, strlen(hello), 0);
    std::cout << "Message sent\n";

    // ECHO CLIENT RECEIVE ORIGINAL MESSAGE
    char buffer[1024] = {0};
    int bytes_received = read(sock, buffer, sizeof(buffer));
    if (bytes_received > 0) {
        std::cout << "Echo from server: " << buffer << "\n";
    }

    // Send data again
    send(sock, message_two, strlen(message_two), 0);
    std::cout << "Message 2 sent\n";

    // ECHO CLIENT RECEIVE ORIGINAL MESSAGE
    bytes_received = read(sock, buffer, sizeof(buffer));
    if (bytes_received > 0) {
        std::cout << "Echo from server: " << buffer << "\n";
    }

    // Close socket
    close(sock);
    return 0;
}

/*
Compile files
g++ -o server server.cpp
g++ -o client client.cpp

Run server
./server

Run client in another terminal
./client
*/