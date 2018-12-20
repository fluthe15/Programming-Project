// Bare_serv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

boolean checkPlacement(string Tile_);
void sendChat(string msg_);
int checkForWin();
void switchTurn();






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
	else if (wsOK == 0)
	{
		cerr << "winsock initialized!" << endl;
	}


	// next we create a socket to bind to
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //TCP specific arguments
	if (listening == INVALID_SOCKET)
	{
		cerr << "cannot create socket!" << endl;
		return 1; // if we cannot create a socket for listening, throw error!
	}
	else if (listening != INVALID_SOCKET)
	{
		cerr << "Socket created!" << endl;
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

				//Message to self that connection is established
				cerr << "Connection with client established!" << endl;

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

				// switch(bytesIn)
				// case "P1,A1":
				// player one asks for a1, so run checkPlacement(Tile)
				// etc..
				// case "P1,text: ....":
				// its a chat message, push it to other client, so run sendChat(Msg)
				// etc...


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
/*
boolean checkPlacement(string Tile_) 
{
	// check if tile is available, or occupied
	// allow placement
	// run switchTurn()
}

void sendChat(string msg_) 
{
	// send msg to both clients, include username??
}

int checkForWin() 
{
	// if 0, player one won, if 1 player 2 won
	// send "victory" or "defeat" based on return statement..
	// if all tiles occupied and no win, restart game....
}

void switchTurn() 
{
	// bool != bool
	// possibly send msg to users... / sendChat("not your turn");
}
*/