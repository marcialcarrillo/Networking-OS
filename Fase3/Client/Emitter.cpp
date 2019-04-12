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

static pthread_mutex_t my_mutex;


void* recvTcp(void* data)
{
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);

	while(1)
	{ 
		//Waits for a image to be done
		Mailbox::mesg_ackEmisor msg;
		childsCopiedData->client->client_read(&msg,sizeof(msg));
		//Server sends the id of the image and Emisor sends it to the contratista
		handshakeMail.sendAck(msg.id);
		DEBUG_MSG("Sent a completed image signal to " << msg.id);
		
	}
	
	return NULL;
}

void* sendChunk(void* data)
{
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);


	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_buffer msg = chunkMail.receive(0,true);
     if (errno != ENOMSG)
     {
			//Creates the header
			char header[1];
			header[0] = '1';
			
			//Mutex my_lock
			 pthread_mutex_lock(&my_mutex);
			
			
			//Sends the 2 packs to the server. First the header
			childsCopiedData->client->client_send(header,sizeof(header));
			 DEBUG_MSG("header1");
			childsCopiedData->client->client_send(&msg,sizeof(msg));
			  DEBUG_MSG("chunk");
			//Mutex Unlock
			 pthread_mutex_unlock(&my_mutex);
     }
    
    
	}
	
	return NULL;
}

void* sendName(void* data)
{
  threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);
	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_name msg = handshakeMail.receiveName(false);//Blocking
    handshakeMail.sendAck(2);
    
    
    //Creates the header
    char header[1];
    header[0] = '0';
    
   	//Mutex my_lock
   pthread_mutex_lock(&my_mutex);
    
    //Sends the 2 packs to the server. First the header
    childsCopiedData->client->client_send(header,sizeof(header));
    DEBUG_MSG("header0");
    childsCopiedData->client->client_send(&msg,sizeof(msg));
	DEBUG_MSG("name" );
    
    //Mutex Unlock
		 pthread_mutex_unlock(&my_mutex);
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
    Client* client = new Client();
    //Client client();	
  

  
  threadData* data = new threadData();
  data->client = client;
  data->handshakeMail = key;
  data->chunkMail = key2;
  
  
	//Starts the receive thread
     pthread_t recieveThread;
        pthread_create(&recieveThread,NULL,recvTcp,(void*)data);
        pthread_detach(recieveThread);
	  
		
		//Starts the sendName thread
        pthread_t sendNameThread;
        pthread_create(&sendNameThread,NULL,sendName,(void*)data);
        pthread_detach(sendNameThread);
	 
	 
	  
	
	//Starts the sendChunks thread
		pthread_t sendChunks;
       pthread_create(&sendChunks,NULL,sendChunk,(void*)data);
      pthread_detach(sendChunks);
	  

	  
while(1)
{
  /*hold process*/
}

  delete(client);

	return 0;
}
