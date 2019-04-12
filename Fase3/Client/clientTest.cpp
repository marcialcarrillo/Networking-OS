#include "Client.h"
#include <iostream>

using namespace std;

    struct mesg_buffer
    {
        long mtype;
        char data[128];
    }buff;

int main()
{
    Client client;
    client.client_read(&buff, sizeof(buff));
    cout << "mtype is: " << buff.mtype << endl;
    client.client_send(&buff,sizeof(buff));
    return 0;
}
