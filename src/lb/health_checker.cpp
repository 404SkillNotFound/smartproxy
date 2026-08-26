#include "backend.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

bool isBackendAlive(const Backend &backend)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1)
        return false;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(std::stoi(backend.port));
    address.sin_addr.s_addr = inet_addr(backend.host.c_str());

    int result = connect(socket_fd, (struct sockaddr *)&address, sizeof(address));

    close(socket_fd);

    return result == 0;
}