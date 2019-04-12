#include <iostream>
#include <cstring>
#include <string>
#include "Client.h"

using namespace std;


void set(char *package, string value)
{
    memset(package, '\0', sizeof(package));
    for (int i = 0; i < value.size(); ++i)
    {
        package[i] = value[i];
    }
}

void sending(char *package, Client& client, string& temp)
{
    cout << "Enviando -> " << temp << endl;
    set(package, temp);
    client.send(package, 10);
}

int main()
{
    Client client;
    string temp("Andres");
    char package[10];

    //client.start();
    sending(package, client, temp);

    temp = "navarrete";
    sending(package, client, temp);

    temp = "boza";
    sending(package, client, temp);

    //client.finish_Client();
    //*/
    return 0;
}

/*

while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_buffer msg = chunkMail.receive(0,false);
    
    //Creates the header
    char header[1];
    header[0] = '1';
    
    //Mutex lock
    
    //Sends the 2 packs to the Client. First the header
    client.send(header,sizeof(header));
    client.send(&msg,sizeof(msg));
    
    send(const void *buf, size_t count)
      
   	 write(this.socket,buf,count);
    
    //Mutex Unlock
	}


*/