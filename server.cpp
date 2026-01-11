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

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach to port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind server to a port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Continuously read data, echo messages back to client
    char buffer[1024] = { 0 };
    while (true) {
        int bytes_read = read(new_socket, buffer, 1024);
        if (bytes_read <= 0) {
            break;
        }   
        std::cout << "Message from client: " << buffer << "\n";
        send(new_socket, buffer, bytes_read, 0);
    }

    // Close socket
    close(new_socket);
    close(server_fd);
    return 0;
}

