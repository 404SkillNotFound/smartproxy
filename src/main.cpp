/*
 * Entry point for smartproxy.
 * Listens for HTTP requests on port 8080, parses each request,
 * selects a backend using least-connections strategy, forwards
 * the request, and relays the response back to the client.
 */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>

#include "parser.hpp"
#include "lb/balancer.hpp"

void die(const std::string &msg)
{
    std::cerr << msg << ": " << strerror(errno) << '\n';
    exit(1);
}

int main()
{
    // Create and configure the main server socket.
    // smartproxy listens on port 8080 using this socket.
    // bind() attaches it to the port and listen() makes it ready
    // to accept incoming connections.
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        die("socket() failed");

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
        die("bind() failed");

    if (listen(server_fd, 6) == -1)
        die("listen() failed");

    Balancer balancer;
    Backend backend1{"127.0.0.1", "5001", "backend1"};
    Backend backend2{"127.0.0.1", "5002", "backend2"};
    Backend backend3{"127.0.0.1", "5003", "backend3"};

    balancer.addBackend(backend1);
    balancer.addBackend(backend2);
    balancer.addBackend(backend3);

    while (true)
    {
        // Accept a connection from a client such as curl.
        // server_fd keeps listening for new connections, while client_fd
        // represents the connection with the specific client we accepted.
        // We use client_fd to receive the request and later send the response back.
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1)
            die("accept() failed");

        char buffer[4096];
        std::string request;

        while (true)
        {
            int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received > 0)
            {
                request.append(buffer, bytes_received);
                if (request.find("\r\n\r\n") != std::string::npos)
                    break;
            }
            else if (bytes_received == 0)
            {
                break;
            }
            else
            {
                die("recv() failed");
            }
        }
        HttpRequest parsedRequest = parseRequest(request);

        Backend& backend = balancer.selectLeastConn();
        backend.active_connections++;

        // Create a separate connection to the Flask backend.
        // smartproxy acts as the client here and connects to the selected Flask backend.
        // backend_fd is used to forward the request to Flask and receive its response.
        int backend_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (backend_fd == -1)
            die("backend() failed");

        sockaddr_in backend_address;
        backend_address.sin_family = AF_INET;
        int backendPort = std::stoi(backend.port);
        backend_address.sin_port = htons(backendPort);
        backend_address.sin_addr.s_addr = inet_addr(backend.host.c_str());

        if (connect(backend_fd, (struct sockaddr *)&backend_address, sizeof(backend_address)) == -1)
            die("backend connect() failed");

        send(backend_fd, request.c_str(), request.size(), 0);
        char responseBuffer[4096];
        std::string backendResponse;

        while (true)
        {
            int bytes_received = recv(backend_fd, responseBuffer, sizeof(responseBuffer), 0);
            if (bytes_received > 0)
            {
                backendResponse.append(responseBuffer, bytes_received);
            }
            else if (bytes_received == 0)
            {
                break;
            }
            else
            {
                die("recv() failed");
            }
        }

        send(client_fd, backendResponse.c_str(), backendResponse.size(), 0);

        std::cout << request << '\n';

        close(backend_fd);
        backend.active_connections--;
        close(client_fd);
    }
    close(server_fd);
}