#include "Mailbox.h"
#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

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
Mailbox::mesg_buffer Mailbox::receive(int type, bool block)
{
    mesg_buffer recv;
    
    if (!block)
    {
        if (msgrcv(this->mailboxId, &recv, sizeof(recv), type, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                //perror("msgrcvId");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        if (msgrcv(this->mailboxId, &recv, sizeof(recv), type, 0) == -1)
        {
            if (errno != ENOMSG)
            {
                //perror("msgrcvId");
                exit(EXIT_FAILURE);
            }
        }
    }
    return recv;
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
Mailbox::mesg_name Mailbox::receiveName(bool block)
{
	int id = msgget(this->key,0666 | IPC_CREAT);
    mesg_name recv;

    if (!block)
    {
        if (msgrcv(id, &recv, sizeof(recv), 4, 0) == -1)
        {
            if (errno != ENOMSG)
            {
                //perror("msgrcv4");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
		
        if (msgrcv(id, &recv, sizeof(recv), 4, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                //perror("msgrcv4");
                exit(EXIT_FAILURE);
            }
            recv.boolean = 1;
        }
        else
        {
			    recv.boolean = 0;
        }
    }
    return recv;
}

void Mailbox::sendAck(int type)
{

    mesg_ack package;
    package.mtype = type;
    package.boolean = 1;
    if (msgsnd(this->mailboxId, &package, sizeof(mesg_ack), 0) == -1)
    {
        //perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
}
//if variable block is set to 1 and theres no message with the variable type then it wont wait for one. Otherwise it will wait for a message
//Return 1 if theres no message to receive. otherwise return 0
Mailbox::mesg_ack Mailbox::receiveAck(int type, bool block)
{
    int id = msgget(this->key, 0666 | IPC_CREAT);
    mesg_ack recv;

    if (block)
    {
        if (msgrcv(id, &recv, sizeof(mesg_ack), type, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                //perror("msgrcvNonBlock");
                exit(EXIT_FAILURE);
            }
            recv.boolean = 1;
        }
        else
          recv.boolean =0;
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

    return recv;
}


void Mailbox::sendAckEmisor(int id)
{
	
	mesg_ackEmisor package;
	package.mtype = 6;
	package.id = id;
	if(msgsnd(this->mailboxId, &package, sizeof(mesg_ackEmisor),0) == -1)
 	{
		//perror("msgsnd error");
        exit(EXIT_FAILURE);
		
	}
 
}

//Return 0 if theres no message to receive. otherwise return id
Mailbox::mesg_ackEmisor Mailbox::receiveAckEmisor()
{
	int id = msgget(this->key,0666 | IPC_CREAT);
	mesg_ackEmisor recv;
		if(msgrcv(id, &recv, sizeof(mesg_ackEmisor),6, 0) == -1)
    	{
			if (errno != ENOMSG) 
			{
				   perror("msgrcvNonBlock");
				   exit(EXIT_FAILURE);
			}
			recv.boolean = 1;
		}
		else
		   recv.boolean = 0;	

		
	  
	return recv;
}