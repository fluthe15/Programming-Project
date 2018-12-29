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
void movex(int row, int col);
void moveo(int row, int col);
int row;
int col;
const int ROWsnum = 3;
const int COLsnum = 3;
char board[COLsnum][ROWsnum] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
bool player = true;
bool boardIsEmpty = true;
bool toKeepLoopRolling = true;

int main()
{
	Client cObject;
	cObject.Connect_To_Server("127.0.0.1", 54000);
	

	//printboard();
	printWelcome();

	cObject.sendMessage("valueOfTurn");
	if (cObject.recvMessage(cObject.buf) == "yes")
	{
		player = true;
	}
	else if (cObject.recvMessage(cObject.buf) == "no")
	{
		player = false;
	}

	

	while (toKeepLoopRolling) 
	{
		cObject.sendMessage("print");

		if (boardIsEmpty == true)
		{
			/*cObject.sendMessage("print");*/
			std::string emptyBoard = cObject.recvMessage(cObject.buf);
			std::cout << emptyBoard << std::endl;
			boardIsEmpty = false;
		}

		// first, get the users input
		std::string userInput; 
		std::getline(std::cin, userInput);

		if (userInput == "A1" || userInput == "a1")
		{
			cObject.sendMessage("a1");
			std::string a1Board = cObject.recvMessage(cObject.buf);
			std::cout << a1Board << std::endl;
		}

		
		

		// then send a request based on that input, and wait for a response!

		//if (cObject.recvMessage(cObject.buf) == "someone won!") 
		//{
		//	// the game is over, show the winner! 
		//}

		//if (userInput == "turn") 
		//{
		//	cObject.sendMessage("valueOfTurn");
		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "yes")
		//	{
		//		std::cout << "Yes it's your turn!" << std::endl;
		//	}
		//	else if (message == "no") 
		//	{
		//		std::cout << "No it's not your turn!" << std::endl;
		//	}

		//	// if it is your turn when you ask the very first time, then you are player 1!!!! 
		//}
		//else if (userInput == "A1" || userInput == "a1" || userInput == "1") 
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("1");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK11" || message == "OK21") 
		//	{
		//		if (player)
		//		{
		//			movex(2, 0);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(2, 0);
		//			printboard();
		//		}
		//		
		//	}
		//	else if (message == "NO1" || message == "NO2") 
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!") 
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "A2" || userInput == "a2" || userInput == "2")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("2");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK12" || message == "OK22")
		//	{
		//		if (player)
		//		{
		//			movex(2, 1);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(2, 1);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "A3" || userInput == "a3" || userInput == "3")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("3");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK13" || message == "OK23")
		//	{
		//		if (player)
		//		{
		//			movex(2, 2);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(2, 2);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "B1" || userInput == "b1" || userInput == "4")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("4");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK14" || message == "OK24")
		//	{
		//		if (player)
		//		{
		//			movex(1, 0);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(1, 0);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "B2" || userInput == "b2" || userInput == "5")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("5");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK15" || message == "OK25")
		//	{
		//		if (player)
		//		{
		//			movex(1, 1);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(1, 1);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "B3" || userInput == "b3" || userInput == "6")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("6");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK16" || message == "OK26")
		//	{
		//		if (player)
		//		{
		//			movex(1, 2);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(1, 2);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "C1" || userInput == "c1" || userInput == "7")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("7");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK17" || message == "OK27")
		//	{
		//		if (player)
		//		{
		//			movex(0, 0);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(0, 0);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "C2" || userInput == "c2" || userInput == "8")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("8");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK18" || message == "OK28")
		//	{
		//		if (player)
		//		{
		//			movex(0, 1);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(0, 1);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}
		//else if (userInput == "C3" || userInput == "c3" || userInput == "9")
		//{
		//	// we ask the server to place X/0 at 1
		//	// and wait for response
		//	cObject.sendMessage("9");

		//	std::string message = cObject.recvMessage(cObject.buf);
		//	if (message == "OK19" || message == "OK29")
		//	{
		//		if (player)
		//		{
		//			movex(0, 2);
		//			printboard();
		//		}
		//		else if (!player)
		//		{
		//			moveo(0, 2);
		//			printboard();
		//		}

		//	}
		//	else if (message == "NO1" || message == "NO2")
		//	{
		//		// that spot is taken
		//	}
		//	else if (message == "not your turn!")
		//	{
		//		// its not your turn
		//	}
		//}

		
	}

}

void printboard()
{
	/*std::cout << board[0][2] << " | " << board[1][2] << " | " << board[2][2] << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << board[0][1] << " | " << board[1][1] << " | " << board[2][1] << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << board[0][0] << " | " << board[1][0] << " | " << board[2][0] << std::endl;*/

	std::cout << " x | x | x \n ---------\n x | x | x \n ---------\n x | x | x " << std::endl;
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