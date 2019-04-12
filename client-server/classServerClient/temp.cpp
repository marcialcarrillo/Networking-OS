#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

struct mesg_buffer
{
    long mtype = 1;
    char data[5];

    // El package _size TIENE que ser menor al tamano de data
    void llenarData(char *package)
    {
        memset(&data, '0', sizeof(data));
        for (int i = 0; i <= 5; ++i)
        {
            data[i] = package[i];
        }
    }

    char* serializar
};

int main()
{
    mesg_buffer buffer;

    char package[5];

    for (int i = 0; i < 5; ++i)
    {
        package[i] = ('a' + i);
    }

    buffer.llenarData(package);
    
    cout << buffer.mtype << endl;
    cout << buffer.data << endl;

    return 0;
}