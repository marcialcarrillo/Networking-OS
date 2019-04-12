#include <stdio.h>      // standard input and output library
#include <stdlib.h>     // this includes functions regarding memory allocation
#include <string.h>     // contains string functions
#include <errno.h>      //It defines macros for reporting and retrieving error conditions through error codes
#include <time.h>       //contains various functions for manipulating date and time
#include <unistd.h>     //contains various constants
#include <sys/types.h>  //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h>  // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses
#include <stdio.h>
#include <string>

using namespace std;

class Client
{
  private:
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

    int port = 0, clientListening = 0, clientConnection = 0;
    string information_file;
    struct sockaddr_in ipOfClient;
    char package[1024];
    int number_clients = 20;

    int getData();
    void set_up_socket();

  public:
    Client();
    ~Client();

    void start();
    void send(char *package, int package_size);
    void finish_Client();
};

Client::Client()
{
    this->getData();
    this->set_up_socket();
}

Client::~Client()
{
    // Body of the destructor
}

int Client::getData()
{
    char line[256], finalLine[256];      // donde se guarda la linea del file y el producto final de lo que se encuentra en el archivo
    FILE *file;                          // Archivo de datos de ip
    file = fopen("clientData.txt", "r"); // Para averiguar la ip

    //For the ip --- getting the ip from the file
    fgets(line, sizeof(line), file);
    memset(finalLine, '\0', sizeof(finalLine)); // llena de '\0' el buffer
    strncpy(finalLine, line, strlen(line) - 1);

    this->port = atoi(finalLine); // char* to integer

    fclose(file);
    return port;
}

// Setting important values to the TCP socket
void Client::set_up_socket()
{
    /*
        Creating a socket
        "AF_INET" ------> because is ipv4 address
        "SOCK_STREAM" --> because we are using TCP protocol
        "0" ------------> this is so that the kernel uses the default protocol
    */
    this->clientListening = socket(AF_INET, SOCK_STREAM, 0);

    // This is to fill with 0's the first part of the ipOfClient and the package
    memset(&(this->ipOfClient), '0', sizeof(this->ipOfClient));
    memset(this->package, '0', sizeof(this->package));

    // Setting important data to the struture ipOfClient in order to listen according to the port and type of connection
    this->ipOfClient.sin_family = AF_INET;
    this->ipOfClient.sin_addr.s_addr = htonl(INADDR_ANY); // converts the unsigned integer hostlong from host byte order to network byte order.
    this->ipOfClient.sin_port = htons(this->port);        // this is the port number of running Client

    /*
        When a socket is created with socket(), it exists in a name space (address family) but has no address assigned to it.
        bind() assigns the address specified by sockaddr to the socket referred to by the file descriptor clientListening.
        ipOfClient specifies the size, in bytes, of the address structure pointed to by addr.
        Traditionally, this operation is called “assigning a name to a socket”.
    */
    bind(this->clientListening, (struct sockaddr *)&(this->ipOfClient), sizeof(this->ipOfClient)); // Assigning a name to a socket

    // Only 20 queue process can be handle by the Client
    listen(this->clientListening, 20); // 20 is the maximun number of clients that can connect to this Client
}

//Start the listening by the Client
void Client::start()
{
    printf("Client running waiting for Server to contact me ....\n");
}

void Client::send(char *package, int package_size)
{
    // This returns the descriptor once the Client is hit by the client
    clientConnection = accept(clientListening, (struct sockaddr *)NULL, NULL);
    write(clientConnection, package, package_size); // enviando por medio del socket
}

void Client::finish_Client()
{
    close(clientConnection); // Closes a file descriptor, so that it no longer refers to any file and may be reused.
}

