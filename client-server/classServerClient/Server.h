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

class Server
{
  private:
    int createdSocket = 0, n = 0;
    char package[1024];
    struct sockaddr_in ipOfServer;
    char ip[100];
    bool problems = false;

    void getData();
    void set_up_socket();

  public:
    // structure to message a package
    struct mesg_buffer
    {
        long mtype;
        char data[128];
    };
    // structure to message a name of file
    struct mesg_name
    {
        long mtype;
        long totalPack;
        char name[62];
        int id;
    };
    // struct for message ack
    struct mesg_ack
    {
        long mtype;
        int boolean;
    };

    Server();
    ~Server();

    void start();
    void listen();
    void talk(char *package);
};

Server::Server()
{
    this->getData();
    // Setting with 0's in all the package length
    memset(this->package, '0', sizeof(this->package));
    //Looking for problems with the socket
    if ((createdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket not created \n");
        this->problems = true;
    }
    this->set_up_socket();
}

Server::~Server()
{
    // Body of the destructor
}

void Server::getData()
{
    FILE *file;
    file = fopen("serverData.txt", "r");

    //For the ip
    memset(this->ip, '\0', sizeof(this->ip));
    fgets(this->ip, sizeof(this->ip) + 2, file);

    fclose(file);
}

// Setting important values to the TCP socket
void Server::set_up_socket()
{
    // Setting up the information
    this->ipOfServer.sin_family = AF_INET;            // Because we are using ipv4 address
    this->ipOfServer.sin_port = htons(2017);          // Port number of the running server
    this->ipOfServer.sin_addr.s_addr = inet_addr(ip); // Port of the Server

    //Assigning a name to the Server socket
    if (connect(createdSocket, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer)) < 0)
    { // If there is no Server socket connection this program is going to end
        printf("Connection failed due to port and ip problems\n");
        this->problems = true;
    }
}

void Server::listen()
{
    // This will read from the outside
    int n = read(createdSocket, package, sizeof(package) - 1);
    this->package[n] = 0;
    puts(package);
}

void Server::talk(char *package)
{
    // This will read from the outside
    // read(createdSocket, package, sizeof(package) - 1);
}

//Start the listening by the Server
void Server::start()
{
    if (!this->problems)
    {
        this->listen();
    }
}

