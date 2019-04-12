
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

using namespace std;

typedef struct threadData{
	
	queue <vector<char>> data;
	int id;
	string name;
	long totalBytes;
	int handshakeMail;
	int chunkMail;
	
}threadData_t;

void* child(void* data)
{
	threadData_t* childsCopiedData = (threadData_t*) data;
	Mailbox handshakeMail = Mailbox(childsCopiedData->handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData->chunkMail);
	
	//Open file
	
	string fileName = childsCopiedData->name.substr(childsCopiedData->name.find_last_of("/")+1);
	ofstream output_file(fileName, ios::binary);
	
	int numberOfComplete512BytesPackets = floor((double)childsCopiedData->totalBytes/(double)512);
	int leftOver128BytesPackets = ceil((childsCopiedData->totalBytes % 512)/(double)128);

	vector<char> retrievedPacket;
	vector<char> packetToAppend;
	//char * myTemp;
	
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



	
	//Sends ack(id)
	handshakeMail.sendAckEmisor(childsCopiedData->id);

	//Exit file
	output_file.close();
	delete(childsCopiedData);
	
	return NULL;
}


int main()
{	
	//Creates the mailboxes
	key_t key = 0xa62074;
	Mailbox handshakeMail = Mailbox(key);

	key_t key2 = 0xb62074;
	Mailbox chunkMail = Mailbox(key2); 
	
	
	
	int workDone=0;
	while(1)
	{
		
		//Waits for ack(5) non blocking meaning it will not stop waiting for a the message
		Mailbox::mesg_ack msg = handshakeMail.receiveAck(5,true);
		if(msg.boolean != 1)//Theres a message
			break;
			
			
		
		//Receive mail type 4
    	Mailbox::mesg_name name = handshakeMail.receiveName(true);//Nonblocking
    	if(name.boolean != 1)//Theres a msg 
    	{
		
			//Sends ack(2)
			handshakeMail.sendAck(2);
		
		
		//threadData_t childsCopiedData = new threadData_t;
		threadData_t* childsCopiedData = new threadData_t();
		childsCopiedData->handshakeMail = key;
		childsCopiedData->chunkMail = key2;
		childsCopiedData->name = name.name;
		childsCopiedData->totalBytes = name.totalPack;
		childsCopiedData->id = name.id;
		

        pthread_t thread;
        pthread_create(&thread,NULL,child,(void*)childsCopiedData);
        pthread_detach(thread);
		}
		
        //Checks if any child is done 
        Mailbox::mesg_ackEmisor test  = handshakeMail.receiveAckEmisor();
        if(test.boolean != 1)
		  handshakeMail.sendAck(test.id);
			
			
			


    }	
	return 0;
}
