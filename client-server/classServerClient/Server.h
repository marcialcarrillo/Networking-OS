#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

class Server
{
  private:
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

  public:
    Server();
    ~Server();

    bool stop = true;

    void server_read(void* new_buffer, int new_buffer_size);
    void server_send(char *package, int package_size);
    void simple_read(char* new_buffer, int new_buffer_size);
    void ssimple_send(char *package, int package_size);
};

Server::Server()
{
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
}

void Server::simple_read(char* new_buffer, int new_buffer_size)
{
    //this->valread = read(new_socket, buffer, 1024);
    this->valread = read(new_socket, new_buffer, new_buffer_size);
    if (valread)
    {
        printf("%s\n", buffer);
    }
}

void Server::server_read(void* new_buffer, int new_buffer_size)
{
    //this->valread = read(new_socket, buffer, 1024);
    this->valread = read(new_socket, new_buffer, new_buffer_size);
    if (valread)
    {
        printf("%s\n", buffer);
    }
}

void Server::server_send(char *package, int package_size)
{
    send(new_socket, package, package_size, 0);
    printf("servidor envió un paquete\n");
}


