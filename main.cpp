#include <iostream>
#include <sys/socket.h> //if you run "man socket" in linux it will give you list of perameters and dependancies :D
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>

using namespace std;

int main(){

    //****create a socket****
    int listeningSocket = socket(AF_INET, SOCK_STREAM,0);   //domain=AF_INET which is standard IPv4 protocols,type=SOCK_STREAM which is a sequenced 2 way comminication of byte streams,protocol=0 means the network stack figures it out itself
    if(listeningSocket == -1){//means socket was not created / does not exist
        cerr<<"Can't create a socket!";//cerr is used only for errors, is unbuffered and never needs to be flushed
        return -1;
    }
    
    //****bind the socket to an ip / port****
    sockaddr_in hint; //creates x of type sockaddr_in which is used for ipv4
    hint.sin_family = AF_INET;//sets the sockets protocols
    hint.sin_port = htons(54000);//host to network short | different architectures use different byte orders, network protocols use big-endian so we need to account for it. htons flips bits across for this purpose
    inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr); //using perameter 0.0.0.0 will give us any address //inet_pton is an internet command that converts a number to an array of integers | 127.0.0.1 - local addr takes 4 numbers separated to dot -> to convert this to string we pass this through the inet_pton() function


    if (bind(listeningSocket, (struct sockaddr*)&hint, sizeof(hint)) == -1) { //binds it, returns int, if 1, error
        cerr<<"Cant bind to IP/Port";
        return -2;
    }

    //****Mark the socket for listening in****
    if (listen(listeningSocket,SOMAXCONN)==-1){//marks the socket for listening, 2nd arguement defines the maximum length to which the queue of pending connections for sockfd may grow and SOMAXCONN is the maximum ammount
        cerr<<"Cant Listen";
        return -3;
    }


    //accept a call
        //creates a socket address for the client
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

        //buffers we will put host and service names in
    char host[NI_MAXHOST];//maximum available host
    char svc[NI_MAXSERV];//maximum available service

    int clientSocket = accept(listeningSocket, 
                                (struct sockaddr*)&client,
                                &clientSize);//creates client socket | (socket being used, client , size)

    if(clientSocket == -1){
        cerr<<"problem with client connecting!";
        return -4;
    }
    //close the listening socket
    close(listeningSocket);//can close listening socket as connection has been established



    //cleans the memory used by host and SVC 
    memset(host,0,NI_MAXHOST);//fills all host with 0's
    memset(svc,0,NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client,clientSize,host,NI_MAXHOST,svc,NI_MAXSERV,0);//!! may have to use sizeOf(client) instead for 3rd param


    if(result){
        cout<<"Host connected on"<<svc<<endl;
    }else{
        inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);//network to port conversion (opposet of pton)
        cout<<host<<"connected on"<<ntohs(client.sin_port)<<endl;
    }

    //while recieving- display message, echo message
    char buf[4096]; //buffer to store incoming data

    while(true){
        //clear buffer
        memset(buf,0,4096);
        //wait for message
        int bytesRecv = recv(clientSocket, buf, 4096,0);//reads 4096 bytes into buffer (buf) from socket "clientSocket"
        if (bytesRecv == -1){
            cerr<<"There was a connection Issue"<<endl;
            break;
        }

        if (bytesRecv == 0){
            cout<<"Client disconnected"<<endl;
        }
        //display message
        cout<<"Recieved: "<<string(buf,0,bytesRecv)<<endl;
        //resend message
        send(clientSocket,buf,bytesRecv+1,0);
        //
    }
    //close socket
    close(clientSocket);


    return 0;
}