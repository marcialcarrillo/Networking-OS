#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

using namespace std;

class Client
{
  private:
    int createdSocket = 0, n = 0;
    char package[1024];
    struct sockaddr_in ipOfClient;
    char ip[100];
    bool problems = false;

    void getData();
    void set_up_socket();

  public:
    Client();
    ~Client();

    void start();
    void listen();
    void talk(char *package);
};

Client::Client()
{
    this->getData();
    memset(this->package, '0', sizeof(this->package)); // Setting with 0's in all the package length
    //Looking for problems with the socket
    if ((createdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket not created \n");
        this->problems = true;
    }
    this->set_up_socket();
}

Client::~Client()
{
    // Body of the destructor
}

void Client::getData()
{
    FILE *file;
    file = fopen("clientData.txt", "r");

    //For the ip
    memset(this->ip, '\0', sizeof(this->ip));
    fgets(this->ip, sizeof(this->ip) + 2, file);

    fclose(file);
}

// Setting important values to the TCP socket
void Client::set_up_socket()
{
    // Setting up the information
    this->ipOfClient.sin_family = AF_INET;            // Because we are using ipv4 address
    this->ipOfClient.sin_port = htons(2017);          // Port number of the running server
    this->ipOfClient.sin_addr.s_addr = inet_addr(ip); // Port of the client

    //Assigning a name to the client socket
    if (connect(createdSocket, (struct sockaddr *)&ipOfClient, sizeof(ipOfClient)) < 0)
    { // If there is no client socket connection this program is going to end
        printf("Connection failed due to port and ip problems\n");
        this->problems = true;
    }
}

void Client::listen()
{
    // This will read from the outside
    int n = read(createdSocket, package, sizeof(package) - 1);
    this->package[n] = 0;
    puts(package);
}

void Client::talk(char *package)
{
    // This will read from the outside
    //read(createdSocket, package, sizeof(package) - 1);
}

//Start the listening by the Client
void Client::start()
{
    if(!this->problems)
    {
        this->listen();
    }
}
