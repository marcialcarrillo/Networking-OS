#ifndef Mailbox_H
#define Mailbox_H

#include <string>
#include <vector>

using namespace std;
class Mailbox
{
  public:
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
        int boolean;
    };

    Mailbox(key_t);
    ~Mailbox();

    //Send and receive chunks of the image
    void send(int, char *);
	vector<char> receive(int);
    mesg_buffer receiveStruct(int, bool);

    //Send and receive the name of the file
    void sendName(const char *, int, long);
	string receiveName();
    mesg_name receiveNameStruct(bool);

    //Send and receive ack
    void sendAck(int);
	int receiveAck(int,int);
    mesg_ack receiveAckStruct(int, bool);

  private:
    int mailboxId;
    int key;
};

#endif // Mailbox_H
