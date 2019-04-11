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

	string fileName = "./output/";
	fileName += childsCopiedData->name.substr(childsCopiedData->name.find_last_of("/") + 1);
	ofstream output_file(fileName, ios::binary);
	
	int numberOfComplete512BytesPackets = floor((double)childsCopiedData->totalBytes/(double)512);
	int leftOver128BytesPackets = ceil((childsCopiedData->totalBytes % 512)/(double)128);

	vector<char> retrievedPacket;
	vector<char> packetToAppend;
	//char * myTemp;
	
	for(int packageCounter= 0 ; packageCounter < numberOfComplete512BytesPackets ; packageCounter++) //loop until there is less or equal to 4 packets
	{
		retrievedPacket = chunkMail.receive(childsCopiedData->id); //retrives an initial packet
		for(size_t i = 0; i < 3; i++) //since we have guaranteed that there are at least other 3 packets, loop through them.
		{
			packetToAppend = chunkMail.receive(childsCopiedData->id); //take another packet
			retrievedPacket.insert(retrievedPacket.end(),packetToAppend.begin(),packetToAppend.end()); //insert it at the end of the primary packet
		}
		

		output_file.write(retrievedPacket.data(), 512); //write 512 bytes to to file

	}
	//only 3 or less packets to process (or left from the loop above)

	retrievedPacket = chunkMail.receive(childsCopiedData->id); //we know there is at least one package left so always take it out

	for(int packageCounter= 0 ; packageCounter < leftOver128BytesPackets - 1; packageCounter++) // if any left, append them. 
	{		
		packetToAppend = chunkMail.receive(childsCopiedData->id); //take another packet
		retrievedPacket.insert(retrievedPacket.end(),packetToAppend.begin(),packetToAppend.end()); //insert it at the end of the primary packet
	}

	int lastPacketBytes = childsCopiedData->totalBytes % 128; //from the last packet you only need some bytes, not all
	int totatLeftoverBytes = ((leftOver128BytesPackets - 1) * 128) + lastPacketBytes; //calcultate how many bytes in TOTAL are left to write
	output_file.write(retrievedPacket.data(), totatLeftoverBytes); //write them to file

	//Sends ack(3)
	handshakeMail.sendAck(3);

	//Exit file
	output_file.close();
}


int main()
{	
	//Creates the mailboxes
	key_t key = 0xa62074;
	Mailbox handshakeMail = Mailbox(key);

	key_t key2 = 0xb62074;
	Mailbox chunkMail = Mailbox(key2); 
	
	int workDone=0;
	
	while(workDone != 1)
	{
		
		//Waits for ack(5) non blocking meaning it will not stop waiting for a the message
		int checkMessage = handshakeMail.receiveAck(5,1);
		if(checkMessage == 0)//Theres a message
			workDone=1;
		
		//Receive mail type 4
    	string dato = handshakeMail.receiveName();
		//Sends ack(2)
		handshakeMail.sendAck(2);
		
		
		//threadData_t childsCopiedData = new threadData_t;
		threadData_t* childsCopiedData = new threadData_t();
		childsCopiedData->handshakeMail = key;
		childsCopiedData->chunkMail = key2;
		
		//Splits the string to get the data "(name)($totalPacks-id)"
		size_t index = dato.find_first_of("$");
		childsCopiedData->name = dato.substr(0,index);
		string temp = dato.substr(index+1);
		size_t index2 = temp.find_first_of("-");
		childsCopiedData->totalBytes = stol(temp.substr(0,index2));
		childsCopiedData->id = stoi(temp.substr(index2+1));

		//std::cout<<childsCopiedData.name<<" "<<ceil((double)childsCopiedData.totalBytes/(double)128)<<" "<<childsCopiedData.id<<endl;


        pthread_t thread;
        pthread_create(&thread,NULL,child,(void*)childsCopiedData);
        pthread_detach(thread);
/*
		  pid_t forkStatus;
		  forkStatus = fork();//Creates a new process
		  if(forkStatus == 0)//If this is not the main process 
		  {
			child(childsCopiedData);
			exit(0);
		  }	
*/


    }	
	return 0;
}