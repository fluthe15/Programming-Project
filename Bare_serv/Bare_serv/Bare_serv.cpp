// Bare_serv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void checkPlacement(int input_, SOCKET sock);
void sendChat(string msg_);
int checkForWin();
void switchTurn();
boolean turn = false;
// a boolean for each tile..
boolean A1, A2, A3, B1, B2, B3, C1, C2, C3 = false;
fd_set master;
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

	
	FD_ZERO(&master);

	FD_SET(listening, &master);

	int input;

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
				cout << "Connection with client established!" << endl;
				
				//Send a welcome message to the connected client
				//string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
				//send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
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
					/*
					
					//Send message to other clients, and not the listening socket, IN CASE OF CHAT MESSAGE!!!!

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
					*/

					for (int i = 0; i <= master.fd_count; i++) 
					{
						SOCKET outSock = master.fd_array[i];
						ostringstream ss;
						//ss << "USERNAME" << sock << ": " << buf << "\r";
						string strOut = ss.str();
						// we expect only "A1"
						input = 0;
						if (strOut == "A1") 
						{
							cout << "we received an A1" << endl;
							input = 1;
						}
						
						//checkPlacement(input, outSock);
						//send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
				
			}

		}
	}

	WSACleanup();
	
	return 0;
}


void checkPlacement(int input_, SOCKET sock)
{
	char OK[] = "OK";
	char NO[] = "NO";
	char TURN[] = "NO TURN";


	//check which client wants to place something
	if (sock == master.fd_array[0]) 
	{
		// check if it is their turn
		if (turn = false) 
		{
			// check if the tile is occupied
			switch (input_) 
			{
			case 1:
				if (A1 = false)
				{
					// no one is occupying this space, give OK
					send(sock, OK, sizeof(OK), 0);
					// set the tile to occupied
					A1 = true;
					// TODO: CALL CHECKFORWIN
					//switch the turn
					switchTurn();
					break;
				}
				else if (A1 = true) 
				{
				// the tile was occupied, give NO
					send(sock, NO, sizeof(NO), 0);
					break;
				}
				break;
			case 2:
				if (A2 = false) 
				{
				
				}
				break;
			default:
				break;
			}
		}
		else send(sock, TURN, sizeof(TURN), 0);
	}
	//check which client wants to place something
	else if (sock == master.fd_array[1]) 
	{
		// check if it is their turn
		if (turn = true) 
		{
			switch (input_)
			{
			case 1:
				if (A1 = false)
				{
					// no one is occupying this space, give OK
					send(sock, OK, sizeof(OK), 0);
					// set the tile to occupied
					A1 = true;
					// TODO: CALL CHECKFORWIN
					//switch the turn
					switchTurn();
					break;
				}
				else if (A1 = true)
				{
					// the tile was occupied, give NO
					send(sock, NO, sizeof(NO), 0);
					break;
				}
				break;
			default:
				break;
			}
		}
		else send(sock, TURN, sizeof(TURN), 0);
	}
}

void sendChat(string msg_)
{
	
	
	
	// send msg to both clients, include username??
}

int checkForWin()
{
	int result = 3;
	// if three X, return 0
	// else if three 0, return 1
	// else return 2, game reset!
	return result;
	// if 0, player one won, if 1 player 2 won
	// send "victory" or "defeat" based on return statement..
	// if all tiles occupied and no win, restart game....
}

void switchTurn()
{
	// turn is now the opposite of turn..
	turn != turn;
}


void resetVars() 
{
// reset all booleans, variables, drink, cupholders, levels, skillpoints etc....
}