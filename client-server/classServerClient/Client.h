#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 8080

class Client
{
  private:
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    bool problems = false;

  public:
    Client(/* args */);
    ~Client();

    bool stop = true;

    void client_read();
    void client_send(const void* package, int package_size);
};

Client::Client(/* args */)
{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        this->problems = true;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        this->problems = true;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        this->problems = true;
    }
}

Client::~Client()
{
}

void Client::client_read()
{
    valread = read(sock, buffer, 1024);
    if (valread)
    {
        printf("%s\n", buffer);
    }
}

void Client::client_send(const void* package, int package_size)
{
    send(sock, package, package_size, 0);
    printf("cliente envió un paquete\n");
}