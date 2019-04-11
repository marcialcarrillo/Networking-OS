#include "Mailbox.h"
#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

// structure to message a package
struct mesg_buffer
{
	long mtype;
	char data[128];
};

// structure to message a name of file
struct mesg_name
{
	long mtype;
	long totalPack;
	char name[62];
	int id;
};

// struct for message ack
struct mesg_ack
{
	long mtype;
	int thread_id;
};

Mailbox::Mailbox(key_t key)
{
	this->mailboxId = msgget(key, 0666 | IPC_CREAT);
	this->key = key;
}

Mailbox::~Mailbox()
{

	//destroy the message queue
	//msgctl(this->mailboxId, IPC_RMID, NULL);
}

void Mailbox::send(int type, char *data)
{
	mesg_buffer package;
	package.mtype = type;

	memcpy(package.data, data, 128);

	if (msgsnd(this->mailboxId, &package, sizeof(package), 0) == -1)
	{
		//perror("msgsnd error");
		exit(EXIT_FAILURE);
	}
}

//Needs to free the char*
vector<char> Mailbox::receive(int type) //flag 0 = package contratista
{
	mesg_buffer recv;
	if (msgrcv(this->mailboxId, &recv, sizeof(recv), type, 0) == -1)
	{
		if (errno != ENOMSG)
		{
			//perror("msgrcvId");
			exit(EXIT_FAILURE);
		}
	}
	//   char* test = (char*)malloc(128);
	//   memcpy(test,recv.data,128);
	char *test = recv.data;
	vector<char> datavector(test, test + 128);

	/*
const char* values = recv.data;
const char* end = values + strlen(values);

datavector.insert( datavector.end(), values, end );
	//vector<char> data;   

	*/
	return datavector;
}

void Mailbox::sendName(const char *name, int id, long totalPack)
{
	mesg_name package;
	package.mtype = 4;
	strcpy(package.name, name);
	package.totalPack = totalPack;
	package.id = id;
	if (msgsnd(this->mailboxId, &package, sizeof(package), 0) == -1)
	{
		//perror("msgsnd error");
		exit(EXIT_FAILURE);
	}
}
//Return a string with the name plus the size of the total package datas
string Mailbox::receiveName()
{
	mesg_name recv;
	if (msgrcv(this->mailboxId, &recv, sizeof(recv), 4, 0) == -1)
	{
		if (errno != ENOMSG)
		{
			//perror("msgrcv4");
			exit(EXIT_FAILURE);
		}
	}
	string copy(recv.name);
	copy = copy + "$" + to_string(recv.totalPack) + "-" + to_string(recv.id);
	return copy;
}

void Mailbox::sendAck(int type, int thread_id)
{

	mesg_ack package;
	package.mtype = type;
	package.thread_id = thread_id;
	if (msgsnd(this->mailboxId, &package, sizeof(mesg_ack), 0) == -1)
	{
		//perror("msgsnd error");
		exit(EXIT_FAILURE);
	}
}
//if variable block is set to 1 and theres no message with the variable type then it wont wait for one. Otherwise it will wait for a message
//Return 1 if theres no message to receive. otherwise return 0
int Mailbox::receiveAck(int type, bool block)
{
	int id = msgget(this->key, 0666 | IPC_CREAT);
	mesg_ack recv;
	int returnValue = 0;

	if (block)
	{
		if (msgrcv(id, &recv, sizeof(mesg_ack), type, IPC_NOWAIT) == -1)
		{
			if (errno != ENOMSG)
			{
				//perror("msgrcvNonBlock");
				exit(EXIT_FAILURE);
			}
			returnValue = -1;
			return returnValue;
		}		
	}
	else
	{
		if (msgrcv(id, &recv, sizeof(mesg_ack), type, 0) == -1)
		{
			if (errno != ENOMSG)
			{
				//perror("msgrcvBLOCK");
				exit(EXIT_FAILURE);
			}
		}
	}
	returnValue = recv.thread_id;
	return returnValue;
}
