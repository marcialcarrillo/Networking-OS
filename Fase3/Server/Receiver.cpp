#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <pthread.h>

#include "Server.h"
#include "Mailbox.h"

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
	
	queue <vector<char>> data;
	int id;
	string name;
	long totalBytes;
	int handshakeMail;
	int chunkMail;
  Server* server;
	
};

void* child(void* data)
{
	threadData* childsCopiedData = (threadData*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);
	DEBUG_MSG("Child " << childsCopiedData->id << " successfully started");
	
	//Open file
	
	string fileName = childsCopiedData->name.substr(childsCopiedData->name.find_last_of("/")+1);
	ofstream output_file(fileName, ios::binary);
	
	int numberOfComplete512BytesPackets = floor((double)childsCopiedData->totalBytes/(double)512);
	int leftOver128BytesPackets = ceil((childsCopiedData->totalBytes % 512)/(double)128);

	vector<char> retrievedPacket;
	vector<char> packetToAppend;
	//char * myTemp;
	
	DEBUG_MSG("Starting child " << childsCopiedData->id << " main packet writting loop");
	for(int packageCounter= 0 ; packageCounter < numberOfComplete512BytesPackets ; packageCounter++) //loop until there is less or equal to 4 packets
	{
		Mailbox::mesg_buffer msg = chunkMail.receive(childsCopiedData->id,true); //retrives an initial packet
		vector<char> datavector(msg.data,msg.data+128);
		retrievedPacket = datavector;
		for(size_t i = 0; i < 3; i++) //since we have guaranteed that there are at least other 3 packets, loop through them.
		{
			Mailbox::mesg_buffer msg2 = chunkMail.receive(childsCopiedData->id,true); //take another packet
			vector<char> datavector2(msg2.data,msg2.data+128);
			packetToAppend = datavector2;
			
			retrievedPacket.insert(retrievedPacket.end(),packetToAppend.begin(),packetToAppend.end()); //insert it at the end of the primary packet
		}
		

		output_file.write(retrievedPacket.data(), 512); //write 512 bytes to to file

	}
	//only 3 or less packets to process (or left from the loop above)

	Mailbox::mesg_buffer msg = chunkMail.receive(childsCopiedData->id,true); //we know there is at least one package left so always take it out
	vector<char> datavector(msg.data,msg.data+128);
	retrievedPacket = datavector;

	for(int packageCounter= 0 ; packageCounter < leftOver128BytesPackets - 1; packageCounter++) // if any left, append them. 
	{		
		
		Mailbox::mesg_buffer msg2 = chunkMail.receive(childsCopiedData->id,true); //take another packet
		vector<char> datavector2(msg2.data,msg2.data+128);
		packetToAppend = datavector2;
		
		retrievedPacket.insert(retrievedPacket.end(),packetToAppend.begin(),packetToAppend.end()); //insert it at the end of the primary packet
	}

	int lastPacketBytes = childsCopiedData->totalBytes % 128; //from the last packet you only need some bytes, not all
	int totatLeftoverBytes = ((leftOver128BytesPackets - 1) * 128) + lastPacketBytes; //calcultate how many bytes in TOTAL are left to write
	output_file.write(retrievedPacket.data(), totatLeftoverBytes); //write them to file

	DEBUG_MSG("Child " << childsCopiedData->id << " packet writting finished");
	//Sends ack(id)
	handshakeMail.sendAckEmisor(childsCopiedData->id);

	//Exit file
	output_file.close();
	DEBUG_MSG("Child " << childsCopiedData->id << " about to delete inherited data and exit");
	delete(childsCopiedData);
	
	return NULL;
}

void* ReadTCP(void* data)
{
	DEBUG_MSG("ReadTCP thread started");
  threadData* childsCopiedData = (threadData*) data;
  Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
  Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);
  
  while(1)
  {
    char header[1];
    childsCopiedData->server->server_read(&header,sizeof(header));

    if(header[0] == 0)
    {
			DEBUG_MSG("ReadTCP received a valid namePacket with title " << childsCopiedData->name);
      Mailbox::mesg_name namePacket;
      childsCopiedData->server->server_read(&namePacket,sizeof(namePacket));
      handshakeMail.sendName(namePacket.name,namePacket.id,namePacket.totalPack);
			DEBUG_MSG("ReadTCP forwarded " << childsCopiedData->name << " to the mailbox");
  	}
    else
    {
      Mailbox::mesg_buffer dataPacket;
      childsCopiedData->server->server_read(&dataPacket, sizeof(dataPacket));
      chunkMail.send(dataPacket.mtype, dataPacket.data);
    }

  }  
  
  return NULL;
}

void* SendAcks(void* data)
{
	DEBUG_MSG("SendAcks thread started");
  threadData* childsCopiedData = (threadData*) data;
  Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
  Mailbox::mesg_ackEmisor ackPacket;
  
  while(1)
  {
		DEBUG_MSG("SendAcks waiting for ACK: special ACK that declares an image written correctly");
    ackPacket = handshakeMail.receiveAckEmisor();
    if(ackPacket.boolean != 1)
    {
			DEBUG_MSG("SendAcks received a valid ack");
       childsCopiedData->server->server_send(&ackPacket, sizeof(ackPacket)); //the server wil ONLY send ackPackets, because of this no header is required, they will all be casted into an ack struct
			DEBUG_MSG("SendAcks forwarded the ack");
		}
   
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

		threadData* TCPForwardingData = new threadData();
		TCPForwardingData->handshakeMail = key;
		TCPForwardingData->chunkMail = key2;
	
	//Start the TCP forwarding thread (to mailboxes)
    pthread_t TCPForwarding;
    pthread_create(&TCPForwarding,NULL,ReadTCP,(void*)TCPForwardingData);
    pthread_detach(TCPForwarding);

		threadData* SendAcksData = new threadData();
		SendAcksData->handshakeMail = key;
		SendAcksData->chunkMail = key2;
  
  	//Start the Ack Sender thread (from mailboxes to TCP)
    pthread_t ackSend;
    pthread_create(&ackSend,NULL,SendAcks,(void*)SendAcksData);
    pthread_detach(ackSend);
	
		while(1)
	{
		DEBUG_MSG("Entering main receiver loop");
		//Waits for ack(5) non blocking meaning it will not stop waiting for a the message
      //NO NEED TO STOP ON FASE3
      	/*
		Mailbox::mesg_ack msg = handshakeMail.receiveAck(5,true);
		if(msg.boolean != 1)//Theres a message
			break;
		*/
			
		
		//Receive mail type 4 TAKEN OVER BY INDEPENDENT THREAD
      
    	Mailbox::mesg_name name = handshakeMail.receiveName(false);//blocking
    		
			//Sends ack(2) NOT NEEDED IN PHASE 3
			//handshakeMail.sendAck(2);
		
		
		//threadData childsCopiedData = new threadData;
		threadData* childsCopiedData = new threadData();
		childsCopiedData->handshakeMail = key;
		childsCopiedData->chunkMail = key2;
		childsCopiedData->name = name.name;
		childsCopiedData->totalBytes = name.totalPack;
		childsCopiedData->id = name.id;
		
		//create a child to assemble the image
        pthread_t thread;
        pthread_create(&thread,NULL,child,(void*)childsCopiedData);
        pthread_detach(thread);
		
		
        //Checks if any child is done TAKEN OVER BY INDEPENDET THREAD
      /*
        Mailbox::mesg_ackEmisor test  = handshakeMail.receiveAckEmisor();
        if(test.boolean != 1)
		handshakeMail.sendAck(test.id);
      */

    }	
	return 0;
}
