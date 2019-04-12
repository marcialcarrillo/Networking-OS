#include <iostream>
#include "Client.h"

using namespace std;

int main()
{
    Client client;
    char *message = "hola desde el client";
    while (true)
    {
        client.client_send(message, 20);
        sleep(1);
    }
    //client.client_read();
    return 0;
}