#include "Mailbox.h"

using namespace std;

int main ()
{
    key_t key = 0xa62074;
	Mailbox mail = Mailbox(key);

    while(1)
    {
        mail.receiveStruct(5,1);
        break;

    //retrieve a mail

    //create a header for it

    //send it 

    //create a packet for the mail

    //send it
    
    }




    return 0;
}