// Bare_serv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>


#pragma comment (lib, "ws2_32.lib")

void resetValues();

// here is our buffer, it contains all the data we send back and forth, consider it the stream of information
char buf[4096];

// here is a struct that we use for the 9 tiles, they contain a number, a state, and an owner (e.g. 1, true, player1)
struct Tile
{
	// we have 9 tiles, A1 = 1, A2 = 2 etc.
	int number;
	// true means occupied, false means vacant
	boolean state;
	// owner should reflect which socket/client occupies a tile, if any
	std::string owner;

} tiles[9]; // an array of the tiles



int main()
{
	// here we initialize the specific game logic
	resetValues();




	// here we initialize all the server connections
	// first we initialize winsock
	WSADATA wData;																				// wsastartup data
	// then we make sure we get version 2,2
	WORD version = MAKEWORD(2, 2);																// give us version 2.2
	// we check that the version we ask for exist and has been properly loaded
	int wsOK = WSAStartup(version, &wData);														// does not return a 0 if all is working well
	if (wsOK != 0)
	{
		std::cerr << "winsock could not be initialized!" << std::endl;
		return 1; // if we cant initialize winsock, throw error!
	}


	// next we create a socket to bind to
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //TCP specific arguments
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "cannot create socket!" << std::endl;
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

	// now we need to define our set of sockets so we can accept connections
	// from multiple clients in a 'master set'
	fd_set master;
	// make sure that the memory address of master is empty and ready for us..
	FD_ZERO(&master);
	// now we add the listener to the master set with FD_SET
	FD_SET(listening, &master);


	// at this point we create our infinite loop
	while (true)
	{
		// we dont want to fuck with the master set (the FD set)
		// and it gets destroyed every time we do a select() on it
		// so we create a sacrificial copy...
		fd_set copyOfMaster = master;
		// we need a count of the amount of sockets we are handling
		// select works as a sort of queue of sockets, since we are handling this 
		// synchronously, rather than async..
		// select does many things, what we want to do is read, so thats the one we use
		// and the rest gets a null pointer.. NOTE: we use copy since thats our sacrificial set
		int socketCount = select(0, &copyOfMaster, nullptr, nullptr, nullptr);
		// now we have our sockets inside copy, and we need a way to iterate on them, so for-loop!
		for (int i = 0; i < socketCount; i++)
		{
			// in this loop, we want to distingush our sockets a bit from each other
			// .. so we treat the i'th element of all the sockets in the FD set:
			SOCKET sock = copyOfMaster.fd_array[i];
			// and if that is a listening socket..
			if (sock == listening)
			{
				// we set it up to accept a new CONNECTION from a client socket

				//so first we accept the connection
				//  (here the first nullptr is actually info on the client, so it may
				//   be used for identification for us, look into it!)
				SOCKET client = accept(listening, nullptr, nullptr);

				// and then we add this connection to the master FD SET
				// note here we are using the real set, not the one we sacced!
				FD_SET(client, &master);

			}
			// otherwise if it is not a listening socket..
			else
			{
				// we set it up to accept a new MESSAGE from the client
				// we do that with rev, and we save it as bytes! we get it from sock..
				// (which remember, is only a socket *not* for listening now)
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);

				// now we want to know if there was something in this message
				if (bytesReceived <= 0)
				{
					// if there was nothing at all, we drop the connection..
					// and we erase it from the master FD set
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// otherwise we treat the message depending on our variables
					// and otherwise change things based on the message we received



					// as an example, we can assemble a string with a PREFIX and then the BUFFER
					// using stringstream assembly magic made from bunnyfarts and vomit
					std::ostringstream ss;
					ss << "A USER SAYS" << ": " << buf << "\r\n";
					// we save the stringstream in an actual closed string
					// so we can send that one out instead of just buf alone
					std::string outboundString = ss.str();

					// and then respond to the client (right now we just echo back)

					// because we have more clients now, we need another iteration over them here
					// if we are to send the same thing to more than one client
					for (int i = 0; i < master.fd_count; i++)
					{
						// .. so we treat the i'th element of all the sockets in the FD set:
						SOCKET outSock = master.fd_array[i];
						// and if they are not a listening socket..
						// here we could also do != sock, if we did not want the message
						// to go back to the one that sent it..
						if (outSock != listening)
						{
							send(outSock, outboundString.c_str(), outboundString.size() + 1, 0);
						}
					}
				}
			}

			// so now that we have set up the sockets for the correct usages
			// we can begin to actually send and receive some things through them

		}

	}
	// close the socket when we are done with it
	//closesocket(clientSocket);

	// shutdown/cleanup winsock
	WSACleanup();

	return 0;
}

void resetValues()
{
	// reset all tiles
	for (int i = 0; i < 9; i++)
	{
		tiles[i].number = i;
		tiles[i].state = false;
		tiles[i].owner = "none";
	}

}

/*  // LEGACY

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



*/

