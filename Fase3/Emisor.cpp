
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

using namespace std;

typedef struct threadData
{

	queue<vector<char>> data;
	int id;
	string name;
	long totalBytes;
	int handshakeMail;
	int chunkMail;

} threadData_t;

void child(threadData_t childsCopiedData)
{

	Mailbox handshakeMail = Mailbox(childsCopiedData.handshakeMail);
	Mailbox chunkMail = Mailbox(childsCopiedData.chunkMail);

	//Open file

	string fileName = "./output/";
	fileName += childsCopiedData.name.substr(childsCopiedData.name.find_last_of("/") + 1);
	ofstream output_file(fileName, ios::binary);

	int totalPackets = ceil((double)childsCopiedData.totalBytes / (double)128);
	//std::cout << "totalPackets = " << totalPackets << endl;
	vector<char> myTempPacket;
	//char * myTemp;

	for (int packageCounter = 0; packageCounter < totalPackets - 1; packageCounter++) //all packets but one
	{
		vector<char> retrievedPacket = chunkMail.receive(childsCopiedData.id); //retrives a packet
		output_file.write(retrievedPacket.data(), 128);												 //writes it to file
	}

	int lastPacketBytes = childsCopiedData.totalBytes % 128; //from the last packet you only need some bytes, not all
	vector<char> retrievedPacket = chunkMail.receive(childsCopiedData.id);
	output_file.write(retrievedPacket.data(), lastPacketBytes); //write the remainder bytes in the file.

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

	int workDone = 0;

	while (workDone != 1)
	{

		//Waits for ack(5) non blocking meaning it will not stop waiting for a the message
		int checkMessage = handshakeMail.receiveAck(5, 1);
		if (checkMessage == 0) //Theres a message
			workDone = 1;

		//Receive mail type 4
		string dato = handshakeMail.receiveName();
		//Sends ack(2)
		handshakeMail.sendAck(2);

		//threadData_t childsCopiedData = new threadData_t;
		threadData_t childsCopiedData;
		childsCopiedData.handshakeMail = key;
		childsCopiedData.chunkMail = key2;

		//Splits the string to get the data "(name)($totalPacks-id)"
		size_t index = dato.find_first_of("$");
		childsCopiedData.name = dato.substr(0, index);
		string temp = dato.substr(index + 1);
		size_t index2 = temp.find_first_of("-");
		childsCopiedData.totalBytes = stol(temp.substr(0, index2));
		childsCopiedData.id = stoi(temp.substr(index2 + 1));

		//std::cout<<childsCopiedData.name<<" "<<ceil((double)childsCopiedData.totalBytes/(double)128)<<" "<<childsCopiedData.id<<endl;

		pid_t forkStatus;
		forkStatus = fork(); //Creates a new process
		if (forkStatus == 0) //If this is not the main process
		{
			child(childsCopiedData);
			exit(0);
		}
	}
	return 0;
}
