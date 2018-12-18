// Bare_serv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	// first we initialize winsock
	WSADATA wData;																				// wsastartup data
	WORD version = MAKEWORD(2, 2);																// give us version 2.2

	int wsOK = WSAStartup(version, &wData);														// does not return a 0 if all is working well
	if (wsOK != 0)
	{
		cerr << "winsock could not be initialized!" << endl;
		return 1; // if we cant initialize winsock, throw error!
	}


	// next we create a socket to bind to
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //TCP specific arguments
	if (listening == INVALID_SOCKET)
	{
		cerr << "cannot create socket!" << endl;
		return 1; // if we cannot create a socket for listening, throw error!
	}

	// bind socket to an IP and a port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);																// host-to-network-short
	hint.sin_addr.S_un.S_addr = INADDR_ANY;														// give us any address plz..
	bind(listening, (sockaddr*)&hint, sizeof(hint));											// notice the casting as pointer of hint addr!

	// assign the socket for listening
	listen(listening, SOMAXCONN);																// we want listening to be a listening socket!

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				//Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add the new connection to the list of connected clients
				FD_SET(client, &master);

				//Send a welcome message to the connected client
				string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}

			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				//Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					//Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}

				else
				{
					//Send message to other clients, and not the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}

				//Accept a new message

			}

		}
	}

	WSACleanup();
	
	return 0;
}



// wait for a connection
/*sockaddr_in client;
int clientsize = sizeof(client);

SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);
if (clientSocket == INVALID_SOCKET)
{
	cerr << "invalid socket!" << endl;
}

char host[NI_MAXHOST];																		// client name
char service[NI_MAXSERV];																	// port client is connected on

// zero out the memory
//memset(host, 0, NI_MAXHOST);
//memset(service, 0, NI_MAXSERV);
ZeroMemory(host, NI_MAXHOST);
ZeroMemory(service, NI_MAXSERV);
if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
{
	cout << host << " connected on port " << service << endl;
}
else																						// try and get a name, otherwise just use the ip
{
	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	cout << host << " connected on port " << ntohs(client.sin_port) << endl;
}

// close listening socket
closesocket(listening);
char buf[4096];

while (true)
{
	ZeroMemory(buf, 4096);																	// zero out our buffer
	//memset(buf, 0, 4096);
	// wait for data from client
	int bytesReceived = recv(clientSocket, buf, 4096, 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		cerr << "error when receving!" << endl;
		break;
	}

	if (bytesReceived == 0)
	{
		//cout << "client disconnected, received 0" << endl;
		break;
	}

	// echo message back to client
	cout << buf
		send(clientSocket, buf, bytesReceived + 1, 0);
}
// NOTE: This is meant for a RAW CONNECTION!

// close the socket when we are done with it
//closesocket(clientSocket);
// shutdown/cleanup winsock */