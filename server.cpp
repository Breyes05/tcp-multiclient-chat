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
std::vector<int> clients;
std::mutex clients_mutex;

void broadcast(int sender_socket, const char* message, ssize_t length) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    std::vector<int> dead_clients;
    for (int client: clients) {
        if (client != sender_socket) {
            std::cout << "Message broadcast from client " << sender_socket << ": \n"; // shows up server side
            std::string client_message = "Client " + std::to_string(sender_socket) + ": " + message;
            int client_message_len = length + 10; // edit this later
            ssize_t sent = send(client, client_message.c_str(), client_message_len, 0);
            if (sent <= 0) {
                dead_clients.push_back(client);
            }
        }
    }

    // erase-remove idiom moves dead clients to the back to erase them and resize vector
    for (int dead : dead_clients) {
        close(dead);
        auto new_end = std::remove(clients.begin(), clients.end(), dead);
        clients.erase(new_end, clients.end());
    }
}

// Reads data, echos message back to client and closes
void handle_client(int client_socket) {
    char buffer[1024] = { 0 };
    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }   
        broadcast(client_socket, buffer, bytes_received);
    }
    // call broadcast somewhere in here
    close(client_socket);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(
            std::remove(clients.begin(), clients.end(), client_socket), clients.end()
        );
    }
    // OPTIONAL: broadcast disconnection to everyone
}

int main()
{
    int server_socket, new_client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach to port
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind server to a port
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept connections, thread for each client
    while (true) {
        new_client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_client_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        {
            // add new_client
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(new_client_socket);
        }
        std::thread t(handle_client, new_client_socket);
        t.detach();
    }

    // Close socket
    close(server_socket);
    return 0;
}

/*
Goals:
- Add ability to transmit data longer than buffer
- Add private chatting
- Broadcast disconnection messages
*/