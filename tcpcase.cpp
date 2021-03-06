//tcpCase.cpp
#include "tcpCase.h"

using namespace std;

#define MAXDATASIZE 100

tcpCase::tcpCase(char * _port, char * _addr)
{
    port = _port;
    addr = _addr;

    //Ensure that servinfo is clear
    memset(&hints, 0, sizeof hints); // make sure the struct is empty

    //setup hints
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    //Setup the structs if error print why
    int res;
    if ((res = getaddrinfo(addr,port,&hints,&servinfo)) != 0)
    {
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
    }

    //setup the socket
    if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
    {
        perror("client: socket");
    }

}

tcpCase::~tcpCase()
{
    // TODO Auto-generated destructor stub
}

void tcpCase::startConnection()
{//This goes into the send/rcv loop
    //Connect
    if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close (s);
        perror("Client Connect");
    }

    //We dont need this anymore
    freeaddrinfo(servinfo);

    int numbytes;
    char buf[MAXDATASIZE];
    while (1)
    {
        //Recv & print Data
        numbytes = recv(s,buf,MAXDATASIZE-1,0);
        buf[numbytes]='\0';
        cout << buf;

        //What do I do with the recived data
        manageRecv(numbytes,buf);

        //break if connection closed
        if (numbytes==0)
        {
            cout << "----------------------CONNECTION CLOSED---------------------------"<< endl;

            break;
        }
        cout << "----------------------CONNECTION OPEN---------------------------"<< endl;
    }

}

bool tcpCase::charSearch(char *toSearch, char *searchFor)
{
    int len = strlen(toSearch);
    int forLen = strlen(searchFor); // The length of the searchfor field

    //Search through each char in toSearch
    for (int i = 0; i < len;i++)
    {
        //If the active char is equil to the first search item then search toSearch
        if (searchFor[0] == toSearch[i])
        {
            bool found = true;
            //search the char array for search field
            for (int x = 1; x < forLen; x++)
            {
                if (toSearch[i+x]!=searchFor[x])
                {
                    found = false;
                }
            }

            //if found return true;
            if (found == true)
                return true;
        }
    }

    return 0;
}

bool tcpCase::sendData(char *msg)
{//Send some data
    //Send some data
    int len = strlen(msg);
    int bytes_sent = send(s,msg,len,0);

    if (bytes_sent == 0)
        return false;
    else
        return true;
}

void tcpCase::manageRecv(int numbytes, char * buf)
{
    /*
    *   TODO: Add your code here!
    *   int numbytes: this is the number of bytes in the recived msg
    *   char * buf: this is the message recieved
    */
}
