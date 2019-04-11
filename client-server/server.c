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


// This server sends data to the client

int getData()
{
    char line[256], finalLine[256];      // donde se guarda la linea del file y el producto final de lo que se encuentra en el archivo
    FILE *file;                          // Archivo de datos de ip
    file = fopen("serverData.txt", "r"); // Para averiguar la ip

    //For the ip --- getting the ip from the file
    fgets(line, sizeof(line), file);
    memset(finalLine, '\0', sizeof(finalLine)); // llena de '\0' el buffer
    strncpy(finalLine, line, strlen(line) - 1);

    int port = atoi(finalLine); // char* to integer

    fclose(file);
    return port;
}

/*

#include <netinet/in.h>

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

*/

int main()
{
    time_t clock;       // variable that stores time time from the ISO-C
    char package[1025]; // This is what is sended and where things get stored between client and server
    int clientListening = 0, clientConnection = 0;
    struct sockaddr_in ipOfServer;

    int port = 0;
    port = getData();

    /* 
        Creating a socket
        "AF_INET" ------> because is ipv4 address 
        "SOCK_STREAM" --> because we are using TCP protocol
        "0" ------------> this is so that the kernel uses the default protocol
    */
    clientListening = socket(AF_INET, SOCK_STREAM, 0);

    // This is to fill with 0's the first part of the ipOfServer and the package
    memset(&ipOfServer, '0', sizeof(ipOfServer));
    memset(package, '0', sizeof(package));


    // Setting important data to the struture ipOfServer in order to listen according to the port and type of connection
    ipOfServer.sin_family = AF_INET;
    ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY); //converts the unsigned integer hostlong from host byte order to network byte order.
    ipOfServer.sin_port = htons(port);              // this is the port number of running server

    /*
        When a socket is created with socket(), it exists in a name space (address family) but has no address assigned to it.  
        bind() assigns the address specified by sockaddr to the socket referred to by the file descriptor clientListening.  
        ipOfServer specifies the size, in bytes, of the address structure pointed to by addr.  
        Traditionally, this operation is called “assigning a name to a socket”.
    */
    bind(clientListening, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer)); // Assigning a name to a socket

    // Only 20 queue process can be handle by the server
    listen(clientListening, 20); // 20 is the maximun number of clients that can connect to this server

    while (1)
    {
        //fputs(clientConnection, file);
        printf("Server running ....Waiting for some Client to contact me ....\n");

        // This returns the descriptor once the server is hit by the client
        clientConnection = accept(clientListening, (struct sockaddr *)NULL, NULL); 

        clock = time(NULL);
        snprintf(package, sizeof(package), "%.24s\r", ctime(&clock)); // Concatenating the message to the package
        //write(clientConnection, package, strlen(package));            // Sending the package to the client trough its connection

        //Lo siguiente corresponde al envio de la cantidad de paquetes que van a llegarle proximamente
        snprintf(package, sizeof(package), "%s", "one");
        write(clientConnection, package, strlen(package));
        //Sending several times to the client ---- va a corresponder a la cantidad de paquetes que implican la imagen
        for(int i = 0; i < 5; ++i) {
            snprintf(package, sizeof(package), "%.24s\r", ctime(&clock)); 
            write(clientConnection, package, strlen(package));
            sleep(1);
        }


        //Lo siguiente corresponde al envio de la cantidad de paquetes que van a llegarle proximamente
        snprintf(package, sizeof(package), "%s", "two");
        write(clientConnection, package, strlen(package));
        //Sending several times to the client ---- va a corresponder a la cantidad de paquetes que implican la imagen
        for(int i = 0; i < 5; ++i) {
            snprintf(package, sizeof(package), "%.24s\r", ctime(&clock)); 
            write(clientConnection, package, strlen(package));
            sleep(1);
        }

        close(clientConnection); // Closes a file descriptor, so that it no longer refers to any file and may be reused.
        sleep(1);                // Makes the server went to sleep for 1 second to also avoid the server eat all my cpu

        //puts(package);
    }
    return 0;
}