/*
 * TCP foundation for SmartProxy. Creates a POSIX socket on port 8080,
 * accepts connections in a loop, reads raw HTTP requests via recv(),
 * and sends a valid HTTP/1.1 response back to the client.
 */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>

void die(const std::string &msg)
{
    std::cerr << msg << ": " << strerror(errno) << '\n';
    exit(1);
}

int main()
{
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
    while (true)
    {
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
        std::string body = "SmartProxy says hello, nerd ";
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: " +
                               std::to_string(body.size()) + "\r\n"
                                                             "Connection: close\r\n"
                                                             "\r\n" +
                               body;

        send(client_fd, response.c_str(), response.size(), 0);

        std::cout << request << '\n';

        close(client_fd);
    }
    close(server_fd);
}