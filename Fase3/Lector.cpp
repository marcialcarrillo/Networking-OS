#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <math.h>

#include "Mailbox.h"

#define BUFFER_SIZE 512
#define SUB_BUFFER_SIZE 128

using namespace std;

void contratista(string address, int id, Mailbox &chunkMail, Mailbox &handshakeMail)
{
	
	char name[63];
	strcpy(name, address.c_str());

	ifstream file(address, ios::binary | ios::ate);
	int size = file.tellg();

	file.close();

	int totalBytes = size;
	//int mod = size%128;

	int extraPacketsToSend = ceil((size % BUFFER_SIZE) / (double)SUB_BUFFER_SIZE);
	//cout << "I need to send " << extraPacketsToSend << " extra packets" << endl;

	//Sends the name to the emisor
	handshakeMail.sendName(name, id, totalBytes);
	//Waits for the ack(2)
	handshakeMail.receiveAck(2, 0);

	char buffer[BUFFER_SIZE];
	char sub_buffer[SUB_BUFFER_SIZE];

	ifstream input_file(address, ifstream::binary);

	int mytest = 0;
	while (input_file.read(buffer, BUFFER_SIZE)) //while there is enough information to fill a buffer, copy information in buffer 512
	{
		for (size_t i = 0; i < (BUFFER_SIZE / SUB_BUFFER_SIZE); i++) //split that gatehred information in 4 parts of size SUB_BUFFER_SIZE
		{

			memcpy(sub_buffer, buffer + (i * SUB_BUFFER_SIZE), SUB_BUFFER_SIZE);
			mytest++;
			chunkMail.send(id, sub_buffer);
		}
	}

	for (int i = 0; i < extraPacketsToSend; i++) //there is a remainder information in the main buffer that wasn't written in the while loop, this writes it
	{
		memcpy(sub_buffer, buffer + (i * SUB_BUFFER_SIZE), SUB_BUFFER_SIZE);
		//cout << "Extra packet number " << extraPacketsToSend << " is of size: " <<  sizeof(sub_buffer) << endl;
		chunkMail.send(id, sub_buffer);

		//output_file.write(sub_buffer, SUB_BUFFER_SIZE);
	}

	//waits for the ack(3) from the emisor
	handshakeMail.receiveAck(3, 0);

	//sends the ack(1)
	handshakeMail.sendAck(1);

	input_file.close();
}

int main(int x, char *argv[])
{

	(void)x;
	std::string line = "find ";
	line += argv[1];
	line += " > ./temp.ini";
	system(line.c_str());
	system("grep -P '.(png|jpeg|gif|jpg)' ./temp.ini > ./addresses.ini");

	if (remove("temp.ini") != 0)
		perror("File deletion failed");

	ifstream file;
	file.open("addresses.ini");

	if (!file)
	{
		cerr << "Unable to open file datafile.txt";
		exit(1); // call system to stop
	}

	//Creates the mailboxes
	key_t key = 0xa62074;
	Mailbox handshakeMail = Mailbox(key);

	key_t key2 = 0xb62074;
	Mailbox chunkMail = Mailbox(key2);

	int contratistaId = 2019;
	string address = "";
	int totalWorkers = 0; //Contratistas
	int totalImages = 0;	//Keeping track the images
	int workersDone = 0;	//Counts the workersDone

	while (file >> address) //While theres addresses to read
	{

		totalImages++;
		if (totalWorkers == 2) //If theres 2 contractors active, wait for one of them to finish before creating another one
		{
			handshakeMail.receiveAck(1, 0); //Waits the ack(1)
			totalWorkers--;
			workersDone++;
		}

		pid_t forkStatus;
		forkStatus = fork(); //Creates a new process
		if (forkStatus == 0) //If this is not the main process
		{
			contratista(address, contratistaId, chunkMail, handshakeMail);
			exit(0);
		}

		totalWorkers++;
		contratistaId++;
	}

	while (workersDone < totalImages) //Waits for all the contractors to finish before deleting the mailboxes
	{

		handshakeMail.receiveAck(1, 0);
		workersDone++;
	}

	//Sends ack(5)
	handshakeMail.sendAck(5);

	//Deletes the queues
	msgctl(msgget(0xb62074, 0666 | IPC_CREAT), IPC_RMID, NULL);
	msgctl(msgget(0xa62074, 0666 | IPC_CREAT), IPC_RMID, NULL);

	//Closes the file
	file.close();

	//Remove a temp file
	if (remove("addresses.ini") != 0)
		perror("File deletion failed");

	return 0;
}
