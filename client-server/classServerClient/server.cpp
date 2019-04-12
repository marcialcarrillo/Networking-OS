#include <iostream>
#include "Server.h"

using namespace std;

struct str_1 {
    char new_buffer[20];
    int new_buffer_size = 20;
} structure_one;

int main()
{
    Server server;
    //char *message = "hola desde el server";

    #pragma pack(1)
    struct mesg_buffer
    {
        long mtype;
        char data[128];
    } message;
    #pragma pack(0)
    
    // for(int i = 0; i < 5; ++i)
   
        server.server_read(&message, sizeof(message));
        cout << "mtype is " << message.mtype << endl;

    //server.server_send(message, 20);
    return 0;
}