#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include "Mailbox.h"
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <mutex>
#include "Client.h"

//************************ VERBOSE MODE ***********************************************//

#define DEBUG //comment this line only to turn off debug messages

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif //DEBUG

//Example debug message:s
//DEBUG_MSG("Hello" << ' ' << "World!" << 1 );
//DEBUG_MSG("");

//************************ VERBOSE MODE ***********************************************//

using namespace std;


struct threadData{
	int handshakeMail;
	int chunkMail;
	Client* client;
};

pthread_mutex_t my_lock;


void* recvTcp(void* data)
{
  DEBUG_MSG("recvTcp thread started");
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);

  DEBUG_MSG("Entering recvTCP loop");
	while(1)
	{
		//Waits for a image to be done
		Mailbox::mesg_ackEmisor msg;
    childsCopiedData->client->client_read(&msg,sizeof(msg));
		
		//Server sends the id of the image and Emisor sends it to the contratista
		handshakeMail.sendAck(msg.id);
	}
	
	return NULL;
}

void* sendChunk(void* data)
{
  DEBUG_MSG("sendChunk thread started");
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);

  DEBUG_MSG("Entering sendChunk loop");
	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_buffer msg = chunkMail.receive(0,false);
    
    //Creates the header
    char header[1];
    header[0] = '1';
    
    //Mutex my_lock
	pthread_mutex_lock(&my_lock);
    
    
    //Sends the 2 packs to the server. First the header
    childsCopiedData->client->client_send(header,sizeof(header));
    childsCopiedData->client->client_send(&msg,sizeof(msg));
     
    //Mutex Unlock
     pthread_mutex_unlock(&my_lock);
	}
	
	return NULL;
}

void* sendName(void* data)
{
  DEBUG_MSG("sendName thread started");
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);
  DEBUG_MSG("Entering sendName loop");
	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_name msg = handshakeMail.receiveName(false);//Blocking
    
    //Creates the header
    char header[1];
    header[0] = '0';
    
   	//Mutex my_lock
   	pthread_mutex_lock(&my_lock);
    
    //Sends the 2 packs to the server. First the header
    childsCopiedData->client->client_send(header,sizeof(header));
    childsCopiedData->client->client_send(&msg,sizeof(msg));
   
    
    //Mutex Unlock
		pthread_mutex_unlock(&my_lock);
	}
	
	return NULL;
}


int main()
{	
	//Creates the mailboxes
	key_t key = 0xa62074;
	Mailbox handshakeMail = Mailbox(key);

	key_t key2 = 0xb62074;
	Mailbox chunkMail = Mailbox(key2); 
	
	
	//Start Tcp Socket
  DEBUG_MSG("trying to connect");
    Client* client = new Client();
  DEBUG_MSG("connected");
    //Client client();	
  
    if (pthread_mutex_init(&my_lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
  
  threadData* data = new threadData();
  data->client = client;
  data->handshakeMail = key;
  data->chunkMail = key2;

  DEBUG_MSG("threadData has: key1 for handshake: " << data->handshakeMail << " and key2 for chunk:" << data->chunkMail);
  
  
	//Starts the receive thread
  DEBUG_MSG("Starting receive thread");
     pthread_t recieveThread;
        pthread_create(&recieveThread,NULL,recvTcp,(void*)data);
        pthread_detach(recieveThread);
	  
	
	
	//Starts the sendChunks thread
  DEBUG_MSG("Starting sendChunks thread");
     pthread_t sendChunks;
        pthread_create(&sendChunks,NULL,sendChunk,(void*)data);
        pthread_detach(sendChunks);
	  
	
		//Starts the sendName thread
    DEBUG_MSG("Starting sendName thread");
        pthread_t sendNameThread;
        pthread_create(&sendNameThread,NULL,sendName,(void*)data);
        pthread_detach(sendNameThread);

DEBUG_MSG("Entering process hold for main");	  
while(1)
{
  /*hold process*/
}

DEBUG_MSG("Deleting client & ending main");
  delete(client);

	return 0;
}