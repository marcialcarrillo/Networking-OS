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

void getData(char *ip)
{
    FILE *file;
    file = fopen("clientData.txt", "r");

    //For the ip
    memset(ip, '\0', sizeof(ip));
    fgets(ip, sizeof(ip) + 2, file);

    fclose(file);
}

int main()
{
    int CreatedSocket = 0, n = 0;
    char package[1024];
    struct sockaddr_in ipOfServer;

    char ip[100];
    getData(ip);

    memset(package, '0', sizeof(package)); // Setting with 0's in all the package length

    // Creating the client socket in ipv4 address and TCP protocol, and the {0} is to tell the kernel to use the default protocol
    if ((CreatedSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket not created \n");
        return 1;
    }

    // Setting up the information
    ipOfServer.sin_family = AF_INET;            // Because we are using ipv4 address
    ipOfServer.sin_port = htons(2017);          // Port number of the running server
    ipOfServer.sin_addr.s_addr = inet_addr(ip); // Port of the client

    //Assigning a name to the client socket
    if (connect(CreatedSocket, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer)) < 0)
    { // If there is no client socket connection this program is going to end
        printf("Connection failed due to port and ip problems\n");
        return 1;
    }

    // Empezando por la diferenciacion de los paquetes que se van a recibir
    read(CreatedSocket, package, sizeof(package) - 1);
    if(package[0] == 'o') {
        puts("one\n");
    } else {
        puts("two\n");
    }
    //Ahora recibiendo los verdaderos paquetes
    while ((n = read(CreatedSocket, package, sizeof(package) - 1)) > 0)
    {
        printf("n = %d\n", n);
        package[n] = 0;

        // Put what it receives in the stdout but not printed yet
        if (fputs(package, stdout) == EOF) // sends the contents of the package to the terminal stdout of the client
        {
            printf("\nStandard output error");
        }

        printf("\n");
    }

    if (n < 0)
    {
        printf("Standard input error \n");
    }

    return 0;
}