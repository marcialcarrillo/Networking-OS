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
//DEBUG_MSG("")

//************************ VERBOSE MODE ***********************************************//

using namespace std;


typedef struct threadsData{
	Mailbox handshakeMail;
	Mailbox chunkMail;
	Client client;
}threadData_t;

pthread_mutex_t lock;


void* recvTcp(void* data)
{
	while(1)
	{
		//Waits for a image to be done
		//Mailbox::mesg_ack msg = client.recieve();
		
		//Server sends the id of the image and Emisor sends it to the contratista
		//handshakeMail.sendAck(msg.mtype);
	}
	
	return NULL;
}

void* sendChunk(void* temp)
{
  
  threadsData* data = (threadsData*) temp;
	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_buffer msg = data->chunkMail.receive(0,false);
    
    //Creates the header
    char header[1];
    header[0] = '1';
    
    //Mutex lock
	pthread_mutex_lock(&lock);
    
    
    //Sends the 2 packs to the server. First the header
    data->client.send(header,sizeof(header));
    data->client.send(&msg,sizeof(msg));
     
    //Mutex Unlock
     pthread_mutex_unlock(&lock);
	}
	
	return NULL;
}

void* sendName(void* temp)
{
    threadsData* data = (threadsData*) temp;
	while(1) 
	{
		//Takes a package from the mailbox	
    Mailbox::mesg_name msg = data->handshakeMail.receiveName(false);//Blocking
    
    //Creates the header
    char header[1];
    header[0] = '0';
    
   	//Mutex lock
   	pthread_mutex_lock(&lock);
    
    //Sends the 2 packs to the server. First the header
    data->client.send(header,sizeof(header));
    data->client.send(&msg,sizeof(msg));
   
    
    //Mutex Unlock
		pthread_mutex_unlock(&lock);
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
	// Client client();
	//   client.start();
	
  
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
  
  threadData_t* data = new threadData_t();
  data->chunkMail = chunkMail;
  data->client = client;
  
  
	//Starts the receive thread
     pthread_t recieveThread;
        pthread_create(&recieveThread,NULL,recvTcp,(void*)data);
        pthread_detach(recieveThread);
	  
	
	
	//Starts the sendChunks thread
     pthread_t sendChunks;
        pthread_create(&sendChunks,NULL,sendChunk,(void*)data);
        pthread_detach(sendChunks);
	  
	
		//Starts the sendName thread
        pthread_t sendNameThread;
        pthread_create(&sendNameThread,NULL,sendName,(void*)data);
        pthread_detach(sendNameThread);
	  
  
	return 0;
}