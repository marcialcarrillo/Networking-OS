#include <iostream>
#include "Client.h"

using namespace std;

int main()
{
     #pragma pack(1)
    struct mesg_buffer
    {
        long mtype;
        char data[128];
    } message;
    #pragma pack(0)

    Client client;
    //char *message = "hola desde el client";
        message.mtype = 57778886;
 
        client.client_send(&message, sizeof(message));
        cout << "sent" << endl;


    //client.client_read();
    return 0;
}