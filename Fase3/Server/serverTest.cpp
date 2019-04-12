#include "Server.h"

using namespace std;

    struct mesg_buffer
    {
        long mtype;
        char data[128];
    }buff;

int main()
{
    Server server;
    buff.mtype = 777777;
    server.server_send(&buff, sizeof(buff));
    server.server_read(&buff,sizeof(buff));
    return 0;
}
