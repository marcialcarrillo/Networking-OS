#include <iostream>
#include "Server.h"

using namespace std;

int main()
{
    Server server;
    char package[20] = "Andres";
    server.start(package);
    return 0;
}