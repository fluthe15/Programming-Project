#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <sstream>

#define SCK_VERSION 0x0202


using namespace std;

int main()
{
    SOCKET ConSock;
    SOCKET ListenSock;
    SOCKADDR_IN address;
    int addrsize = sizeof(address);

    long ok;
    char MESSAGE[200];

    WSAData WSD;
    WORD DllVersion;
    DllVersion = MAKEWORD(2,1);
    ok = WSAStartup(DllVersion, &WSD);

    ConSock = socket(AF_INET, SOCK_STREAM, NULL);

    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(10102);

    ListenSock = socket(AF_INET, SOCK_STREAM, NULL);
    bind(ListenSock, (SOCKADDR*)&address, sizeof(address));
    listen(ListenSock, SOMAXCONN);

    cout<< "Server waiting for connection\n\n";

    while(true)
    {
        if(ConSock = accept(ListenSock, (SOCKADDR*)&address, &addrsize))
        {
            ok = recv(ConSock, MESSAGE, sizeof(MESSAGE), NULL);

            string msg;
            msg = MESSAGE;
            cout<<"Client says:\t"<<msg<<endl;

            string reply;
            cout<<"Enter reply:\t";
            cin>> reply;

            const char* s = reply.c_str();
            ok = send(ConSock, s, 1024, NULL);

        }
    }

}
