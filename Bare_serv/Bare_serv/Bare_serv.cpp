// Bare_serv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#include <ctype.h>


#pragma comment (lib, "ws2_32.lib")

// some of the methods we are gonna use
void resetValues();
void switchTurn();
void closeClean();
boolean checkForWin();

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

// this boolean controls which players' turn it is
boolean turn;
// this string holds a winner, if any..
std::string winner = "";


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
	else if (wsOK == 0)
	{
		std::cerr << "winsock Initialized!" << std::endl;
	}


	// next we create a socket to bind to
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //TCP specific arguments
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "cannot create socket!" << std::endl;
		return 1; // if we cannot create a socket for listening, throw error!
	}
	else if (listening != INVALID_SOCKET)
	{
		std::cerr << "socket created!" << std::endl;
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

				std::cerr << "Connection established!" << std::endl;

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
				int bytesReceived = 0;
				bytesReceived = recv(sock, buf, 4096, 0);

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


					// first let's save it to a string we can compare to
					//std::string stringBuf = buf;
					// ^this works, but lets try the fancy new ss!^
					std::ostringstream ss;
						ss << buf;
						std::string stringBuf = ss.str();
						
					// now to see if the message was a request for a tile
					// by seeing if we get a digit as the first part of the string
					// (a little unsafe if we get chat working, OBS)
					if(isdigit(stringBuf.at(0)))
					{
						// we create a switchint of the string (the string as an int)
					// and also a controlInt, to save which tile the client wanted..
						int switchInt = std::stoi(stringBuf);
						int controlInt = 0;

						// then we do a switch case to set the controlInt correctly, based on the string..
						switch (switchInt)
						{
						case 1:
							controlInt = 1;
							break;
						case 2:
							controlInt = 2;
							break;
						case 3:
							controlInt = 3;
							break;
						case 4:
							controlInt = 4;
							break;
						case 5:
							controlInt = 5;
							break;
						case 6:
							controlInt = 6;
							break;
						case 7:
							controlInt = 7;
							break;
						case 8:
							controlInt = 8;
							break;
						case 9:
							controlInt = 9;
							break;
						default:
							controlInt = 0;
							break;
						}
						// we set the controlInt according to what the string was as an int..
						// so if controlInt is larger than 0, we know it was a tile they asked for, right?
						if (controlInt > 0)
						{
							// now we can set the proper values of our tiles, based on which
							// client asked for the tile (-1, because 0 is reserved for testing against)

							// is it player 1? (first to connect)
							//if (sock == master.fd_array[2])
							if(turn)
							{
								// if the tile is vacant
								if (tiles[controlInt - 1].state == false)
								{
									std::cerr << "1 Received and placement is OK with turn true!" << std::endl;
									// claim ownership
									tiles[controlInt - 1].owner == "P1";
									// occupy tile
									tiles[controlInt - 1].state == true;
									// give over turn
									send(sock, "OK1", 3, 0);
									switchTurn();
								}
								// if the tile is occupied
								else if (tiles[controlInt - 1].state == true)
								{
									std::cerr << "1 Received and NO! placement with turn true!" << std::endl;
									send(sock, "NO1",3,0);
								}
							}
							// or is it player 2? (second to connect)
							//else if (sock == master.fd_array[3])
							else if(!turn)
							{
								std::cerr << "1 Received and placement is OK with turn false!" << std::endl;
								// if the tile is vacant
								if (tiles[controlInt - 1].state == false)
								{
									// claim ownership
									tiles[controlInt - 1].owner == "P2";
									// occupy tile
									tiles[controlInt - 1].state == true;
									// give over turn
									send(sock, "OK2",3,0);
									switchTurn();
								}
								// if the tile is occupied
								else if (tiles[controlInt - 1].state == true)
								{
									std::cerr << "1 Received and NO! placement with turn false!" << std::endl;
									send(sock, "NO2", 3, 0);
								}
							}

						}
					}
					else 
					{
						// default response..
						send(sock, "response! \r\n", 13, 0);
					}
					/*

					// as an example, we can assemble a string with a PREFIX and then the BUFFER
					// using stringstream assembly magic made from bunnyfarts and vomit
					ss.str("");
					ss << "A USER SAYS" << ": " << buf << "\r\n" << "SERVER SAYS" << ": " << stringBuf << "\r\n";
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


					*/
				}
			}

		}

	}
	return 0;
}

void switchTurn() 
{
	// give the turn over, in effect just switch a boolean
	// true is player 1, false is player 2
	turn = !turn;
	if (checkForWin()) 
	{
		// SOMEBODY WON!!!!
		// so we we send a message to that somebody!
		if (winner == "Player 1") 
		{
		
		}
		else if (winner == "Player 2") 
		{
		
		}
	}
	
}

boolean checkForWin() 
{
	// check for a win condition here
	// we have 8 combinations of tiles that result in win
	// we check them all and see who is the owner
	// in the case of a win...
	boolean aWin = false;
	
	if (tiles[1].state == true && tiles[2].state == true && tiles[3].state == true) 
	{
		if (tiles[1].owner == "P1") 
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[1].owner == "P2") 
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[4].state == true && tiles[5].state == true && tiles[6].state == true)
	{
		if (tiles[4].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[4].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[7].state == true && tiles[8].state == true && tiles[9].state == true)
	{
		if (tiles[7].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[7].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[1].state == true && tiles[4].state == true && tiles[7].state == true)
	{
		if (tiles[1].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[1].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[2].state == true && tiles[5].state == true && tiles[8].state == true)
	{
		if (tiles[2].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[2].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[3].state == true && tiles[6].state == true && tiles[9].state == true)
	{
		if (tiles[3].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[3].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[1].state == true && tiles[5].state == true && tiles[9].state == true)
	{
		if (tiles[1].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[1].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else if (tiles[3].state == true && tiles[5].state == true && tiles[7].state == true)
	{
		if (tiles[3].owner == "P1")
		{
			aWin = true;
			winner = "Player 1";
		}
		else if (tiles[3].owner == "P2")
		{
			aWin = true;
			winner = "Player 2";
		}
	}
	else aWin = false;
	return aWin;
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
	
	// set turn to player 1
	turn = true;
}

void closeClean() 
{
	WSACleanup();
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

