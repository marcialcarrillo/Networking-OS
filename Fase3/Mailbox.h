#ifndef Mailbox_H
#define Mailbox_H

#include <string>
#include <vector>

using namespace std;
class Mailbox
{
    public:
        Mailbox(key_t);
       ~Mailbox();
    
    //Send and receive chunks of the image   
	void send(int, char*);
	vector<char> receive(int);
	
	//Send and receive the name of the file
	void sendName(const char*,int,long);
	string receiveName();
	
	//Send and receive ack
	void sendAck(int,int);
	int receiveAck(int,bool);


   private:
	int mailboxId;
	int key;

		
};



#endif // Mailbox_H
