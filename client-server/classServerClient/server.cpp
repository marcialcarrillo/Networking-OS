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
    char *message = "hola desde el server";
    
    // for(int i = 0; i < 5; ++i)
    while(1)
    {
        server.server_read(structure_one.new_buffer, structure_one.new_buffer_size);
        cout << structure_one.new_buffer << endl;
        cout << structure_one.new_buffer_size << endl;
    }

    //server.server_send(message, 20);
    return 0;
}