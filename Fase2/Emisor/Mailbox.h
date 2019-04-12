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
        int boolean;
    };

    // struct for message ack
    struct mesg_ack
    {
        long mtype;
        int boolean;
    };

    // struct for message ack Emisor
    struct mesg_ackEmisor
    {
        long mtype;
        int boolean;
        int id;
    };

    Mailbox(key_t);
    ~Mailbox();

    //Send and receive chunks of the image
    void send(int, char *);
    mesg_buffer receive(int, bool);

    //Send and receive the name of the file
    void sendName(const char *, int, long);
    mesg_name receiveName(bool);

    //Send and receive ack
    void sendAck(int);
    mesg_ack receiveAck(int, bool);
    
   //Send and receive ackEmisor
    void sendAckEmisor(int);
    mesg_ackEmisor receiveAckEmisor();

  private:
    int mailboxId;
    int key;
};

#endif // Mailbox_H
