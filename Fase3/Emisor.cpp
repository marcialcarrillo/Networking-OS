#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include "Mailbox.h"
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <pthread.h>

//------------------Server---------------------
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#include <sys/socket.h> // Core BSD socket functions and data structures.
#include <netinet/in.h> // AF_INET and AF_INET6 address families and their
                        // corresponding protocol families PF_INET and PF_INET6.
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses.

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
//------------------Server---------------------

using namespace std;

typedef struct threadData{
	
	queue < vector< char > > data;
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
	//------------------Server---------------------
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Get IP
	servaddr.sin_port = htons(PORT);  // Get Port
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

	// Function for chatting between client and server 
	//func(connfd); 

	// After chatting close the socket 
	close(sockfd);
	//------------------Server---------------------


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