#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Client.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

void printboard();
void printWelcome();
const int ROWsnum = 3;
const int COLsnum = 3;
char board[COLsnum][ROWsnum] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };

int main()
{
	Client cObject;
	cObject.Connect_To_Server("127.0.0.1", 54000);
	

	printboard();
	printWelcome();

	while (true) 
	{

		// first, get the users input
		std::string userInput; 
		std::getline(std::cin, userInput);
		

		// then send a request based on that input, and wait for a response!

		if (cObject.recvMessage(cObject.buf) == "someone won!") 
		{
			// the game is over, show the winner! 
		}

		if (userInput == "is it my turn?") 
		{
			//cObject.sendMessage("valueOfTurn")
			std::string message = cObject.recvMessage(cObject.buf);
			if (message == "yes")
			{
				// its your turn
			}
			else if (message == "no") 
			{
				// its not your turn
			}

			// if it is your turn when you ask the very first time, then you are player 1!!!! 
		}
		else if (userInput == "A1" || userInput == "a1" || userInput == "1") 
		{

			// we ask the server to place X/0 at 1
			// and wait for response
			std::string message = cObject.recvMessage(cObject.buf);
			if (message == "OK11" || message == "OK21") 
			{
				// it was successfully placed
			}
			else if (message == "NO11" || message == "NO21") 
			{
				// that spot is taken
			}
			else if (message == "not your turn!") 
			{
				// its not your turn
			}
		}

		
	}

}

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
	std::string welcome = "Welcome to the console based tic tac toe game. \n You should already be connected to the server and ready to begin. \n In order to play, you will have to dictate commands. Try asking \"is it my turn?\" ";
	std::cout << welcome;
}