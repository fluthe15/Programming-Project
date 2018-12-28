#include "Client.h"
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

Client::Client()
{
}


Client::~Client()
{
}

SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
char buf[4096];

void Client::Connect_To_Server(std::string IPADD, int PORT)
			{
				std::string ipAddress = IPADD;			// IP Address of the server
				int port = PORT;						// Listening port # on the server

				// Initialize WinSock
				WSAData data;
				WORD ver = MAKEWORD(2, 2);
				int wsResult = WSAStartup(ver, &data);
				if (wsResult != 0)
				{
					std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
					return;
				}

				// Create socket
				sock = socket(AF_INET, SOCK_STREAM, 0);
				if (sock == INVALID_SOCKET)
				{
					std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
					WSACleanup();
					return;
				}

				// Fill in a hint structure
				sockaddr_in hint;
				hint.sin_family = AF_INET;
				hint.sin_port = htons(port);
				inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

				// Connect to server
				int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
				if (connResult == SOCKET_ERROR)
				{
					std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
					closesocket(sock);
					WSACleanup();
					return;
				}

				//here and up is connection to server

					// Do-while loop to send and receive data
				//char buf[4096];
				//std::string userInput;

				/* do
				{
					/*
					// Prompt the user for some text
					std::cout << "> ";
					getline(std::cin, userInput);

					if (userInput.size() >= 0)		// Make sure the user has typed in something
					{
						// Send the text
						int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
						
						if (sendResult != SOCKET_ERROR)
						{
							// Wait for response
							ZeroMemory(buf, 4096);
							int bytesReceived = recv(sock, buf, 4096, 0);
							if (bytesReceived > 0)
							{
								// Echo response to console
								std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
							}
						}
					}
				
				} while (userInput.size() > 0);

				// Gracefully close down everything
				closesocket(sock);
				WSACleanup();
				*/

			}

void Client::sendMessage(std::string msg_) 
{
	send(sock, msg_.c_str(), msg_.size() + 1, 0);
}

std::string Client::recvMessage(char buf_[4096]) 
{
	bool receiving = true;
	while (receiving) 
	{
		// Wait for response
		ZeroMemory(buf_, 4096);
		int bytesReceived = recv(sock, buf_, 4096, 0);
		// if we received something, stop listening..
		if (bytesReceived > 0) { receiving = false; }
	}
	// convert msg received to string, and return it..
	std::string returnString = buf_;
	return returnString.c_str();
}

void Client::graceDisconnect() 
{
	closesocket(sock);
	WSACleanup();
}