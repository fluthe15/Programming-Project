#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
void printboard();
void printWelcome();
void movex(int row, int col);
void moveo(int row, int col);
int row;
int col;
const int ROWsnum = 3;
const int COLsnum = 3;
char board[COLsnum][ROWsnum] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
bool player = true;

SOCKET sock1 = socket(AF_INET, SOCK_STREAM, 0);
char buffer[4096];
std::string userInput;

int main() 
{
	std::string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return 0;
	}

	// Create socket
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock1 == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock1, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock1);
		WSACleanup();
		return 0;
	}
	else
	{
		std::cout << "Connected to server!" << std::endl;
	}




	do 
	{
		printboard();
		int bytesReceived;
		std::getline(std::cin, userInput);

		if (userInput.size() >= 0) 
		{
			int sendResult = send(sock1, userInput.c_str(), userInput.size() + 1, 0);

			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buffer, 4096);
				bytesReceived = recv(sock1, buffer, 4096, 0);
				/*
				if (bytesReceived > 0)
				{
					std::string result = std::string(buffer, 0, bytesReceived);
					if (result == "OK11") 
					{
						std::cout << "NICE MOVE!" << std::endl;
						movex(2, 0);
					}
				}
				*/
			}
		}
		else if (bytesReceived > 0) 
		{
			std::string result = std::string(buffer, 0, bytesReceived);
			if (result == "OK11")
			{
				movex(2, 0);
				printboard();
			}
			else if (result == "OK22") 
			{
				moveo(2, 1);
				printboard();
			}
		}
	
	} while (userInput.size() > 0);

	//return 0;
}




/*
int main()
{
	Client cObject;
	cObject.Connect_To_Server("127.0.0.1", 54000);
	
	printWelcome();

	cObject.sendMessage("valueOfTurn");
	if (cObject.recvMessage(cObject.buffer) == "yes")
	{
		player = true;
	}
	else if (cObject.recvMessage(cObject.buffer) == "no")
	{
		player = false;
	}

	memset(cObject.buffer, 0,4096);
	boolean request = false;

	while (true) 
	{

		//if (cObject.recvMessage(cObject.buffer) == "someone won!") 
		//{
		//	// the game is over, show the winner! 
		//}

		if (cObject.recvMessage(cObject.buffer).length() > 0) 
		{
			request = false;
			//turn return
			std::string message = cObject.recvMessage(cObject.buffer);
			if (cObject.recvMessage(cObject.buffer) == "yes")
			{
				std::cout << "Yes it's your turn!" << std::endl;
			}
			else if (cObject.recvMessage(cObject.buffer) == "no")
			{
				std::cout << "No it's not your turn!" << std::endl;
			}

			// 1 return
			else if (message == "OK11" || message == "OK21")
			{
				if (player)
				{
					movex(2, 0);
					printboard();
				}
				else if (!player)
				{
					moveo(2, 0);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 2 return
			else if (message == "OK12" || message == "OK22")
			{
				if (player)
				{
					movex(2, 1);
					printboard();
				}
				else if (!player)
				{
					moveo(2, 1);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 3 return
			else if (message == "OK13" || message == "OK23")
			{
				if (player)
				{
					movex(2, 2);
					printboard();
				}
				else if (!player)
				{
					moveo(2, 2);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 4 return
			else if (message == "OK14" || message == "OK24")
			{
				if (player)
				{
					movex(1, 0);
					printboard();
				}
				else if (!player)
				{
					moveo(1, 0);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 5 return
			else if (message == "OK15" || message == "OK25")
			{
				if (player)
				{
					movex(1, 1);
					printboard();
				}
				else if (!player)
				{
					moveo(1, 1);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}
			
			
			
			// 6 return
			else if (message == "OK16" || message == "OK26")
			{
				if (player)
				{
					movex(1, 2);
					printboard();
				}
				else if (!player)
				{
					moveo(1, 2);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 7 return
			else if (message == "OK17" || message == "OK27")
			{
				if (player)
				{
					movex(0, 0);
					printboard();
				}
				else if (!player)
				{
					moveo(0, 0);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 8 return
			else if (message == "OK18" || message == "OK28")
			{
				if (player)
				{
					movex(0, 1);
					printboard();
				}
				else if (!player)
				{
					moveo(0, 1);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}


			// 9 return
			else if (message == "OK19" || message == "OK29")
			{
				if (player)
				{
					movex(0, 2);
					printboard();
				}
				else if (!player)
				{
					moveo(0, 2);
					printboard();
				}

			}
			else if (message == "NO1" || message == "NO2")
			{
				// that spot is taken
			}
			else if (message == "not your turn!")
			{
				// its not your turn
			}
			else request = true;

		}
		else if (cObject.recvMessage(cObject.buffer).length() <= 0 || request == true)
		{
			// first, get the users input
			std::string userInput;
			std::getline(std::cin, userInput);

			if (userInput == "turn")
			{
				cObject.sendMessage("valueOfTurn");				
				// if it is your turn when you ask the very first time, then you are player 1!!!! 
			}
			
			
			else if (userInput == "A1" || userInput == "a1" || userInput == "1")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("1");

				
				
			}
			else if (userInput == "A2" || userInput == "a2" || userInput == "2")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("2");

				
			}


			else if (userInput == "A3" || userInput == "a3" || userInput == "3")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("3");

				
			}


			else if (userInput == "B1" || userInput == "b1" || userInput == "4")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("4");
				

			}


			else if (userInput == "B2" || userInput == "b2" || userInput == "5")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("5");

				
			}

			else if (userInput == "B3" || userInput == "b3" || userInput == "6") 
			{
				cObject.sendMessage("6");
			
			}
			else if (userInput == "C1" || userInput == "c1" || userInput == "7")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("7");
				
			}
			else if (userInput == "C2" || userInput == "c2" || userInput == "8")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("8");

				
			}


			else if (userInput == "C3" || userInput == "c3" || userInput == "9")
			{
				// we ask the server to place X/0 at 1
				// and wait for response
				cObject.sendMessage("9");
				
			}
		}
		

		
	}

}
*/

void printboard()
{
	std::cout << board[0][2] << " | " << board[1][2] << " | " << board[2][2] << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << board[0][1] << " | " << board[1][1] << " | " << board[2][1] << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << board[0][0] << " | " << board[1][0] << " | " << board[2][0] << std::endl;
}

void printWelcome() 
{
	std::string welcome = "Welcome to the console based tic tac toe game. \nYou should already be connected to the server and ready to begin. \nIn order to play, you will have to dictate commands. Try asking \"turn\" ";
	std::cout << welcome << std::endl;
}

void movex(int row, int col)
{
	board[col][row] = 'X';
}

void moveo(int row, int col)
{
	board[col][row] = 'O';
}