#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Client.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

void printboard();

const int ROWsnum = 3;
const int COLsnum = 3;
char board[COLsnum][ROWsnum] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };

int main()
{
	Client cObject;
	cObject.Connect_To_Server("127.0.0.1", 54000);
	std::cout << "You have connected to the server!" << std::endl;

	printboard();


}

void printboard()
{
	std::cout << board[0][2] << " | " << board[1][2] << " | " << board[2][2] << std::endl;
	std::cout << "__________" << std::endl;
	std::cout << board[0][1] << " | " << board[1][1] << " | " << board[2][1] << std::endl;
	std::cout << "__________" << std::endl;
	std::cout << board[0][0] << " | " << board[1][0] << " | " << board[2][0] << std::endl;
}
