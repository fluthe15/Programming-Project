#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "54000"
// the default buffer length for when messeging back and forth
#define DEFAULT_BUFLEN 512

// comment here!
struct clientType
{
	int clientID;
	SOCKET socket;
};

const char OPTION_VALUE = 1;
const int MAX_CLIENT_AMOUNT = 5;

// comment here! - this is declaring the methods below
int processClient(clientType &newClient, std::vector<clientType> &clientArray, std::thread &thread);
int main();

// comment here!
int processClient(clientType &newClient, std::vector<clientType> &clientArray, std::thread &thread)
{
	std::string message = "";
	char tempMessage[DEFAULT_BUFLEN] = "";

	while (1)
	{
		memset(tempMessage, 0, DEFAULT_BUFLEN);

		if (newClient.socket != 0)
		{
			int result = recv(newClient.socket, tempMessage, DEFAULT_BUFLEN, 0);

			if (result != SOCKET_ERROR)
			{
				if (strcmp("", tempMessage))
					message = "Client #" + std::to_string(newClient.clientID) + ": " + tempMessage;

				std::cout << message.c_str() << std::endl;

				for (int i = 0 ; i < MAX_CLIENT_AMOUNT ; i++)
				{
					if (clientArray[i].socket != INVALID_SOCKET)
						if (newClient.clientID != i)
							result = send(clientArray[i].socket, message.c_str(), strlen(message.c_str()), 0);
				}
			}
			else
			{
				message = "Client #" + std::to_string(newClient.clientID) + " Disconnected";

				std::cout << message << std::endl;

				closesocket(newClient.socket);
				closesocket(clientArray[newClient.clientID].socket);
				clientArray[newClient.clientID].socket = INVALID_SOCKET;

				for (int i = 0; i < MAX_CLIENT_AMOUNT; i++)
				{
					if (clientArray[i].socket != INVALID_SOCKET)
						result = send(clientArray[i].socket, message.c_str(), strlen(message.c_str()), 0);
				}

				break;
			}
		}
	} //end while

	thread.detach();

	return 0;
}

int main()
{
	WSADATA wsaData;
	struct addrinfo hints;
	struct addrinfo *server = NULL;
	SOCKET serverSocket = INVALID_SOCKET;
	std::string message = "";
	std::vector<clientType> client(MAX_CLIENT_AMOUNT);
	int numberOfClients = 0;
	int tempID = -1;
	std::thread my_thread[MAX_CLIENT_AMOUNT];

	//Initialize Winsock
	std::cout << "Intializing Winsock..." << std::endl;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Setup hints
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Setup Server
	std::cout << "Setting up server..." << std::endl;
	getaddrinfo(static_cast<LPCTSTR>(IP_ADDRESS), DEFAULT_PORT, &hints, &server);

	//Create a listening socket for connecting to server
	std::cout << "Creating server socket..." << std::endl;
	serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

	//Setup socket options
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
	setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs

	//Assign an address to the server socket.
	std::cout << "Binding socket..." << std::endl;
	bind(serverSocket, server->ai_addr, (int)server->ai_addrlen);

	//Listen for incoming connections.
	std::cout << "Listening..." << std::endl;
	listen(serverSocket, SOMAXCONN);

	//Initialize the client list
	for (int i = 0 ; i < MAX_CLIENT_AMOUNT ; i++)
	{
		client[i] = { -1, INVALID_SOCKET };
	}

	while (1)
	{
		SOCKET incoming = INVALID_SOCKET;
		incoming = accept(serverSocket, NULL, NULL);

		if (incoming == INVALID_SOCKET) continue;

		//Reset the number of clients
		numberOfClients = -1;

		//Create a temporary id for the next client
		tempID = -1;
		for (int i = 0; i < MAX_CLIENT_AMOUNT; i++)
		{
			if (client[i].socket == INVALID_SOCKET && tempID == -1)
			{
				client[i].socket = incoming;
				client[i].clientID = i;
				tempID = i;
			}

			if (client[i].socket != INVALID_SOCKET)
				numberOfClients++;

			//std::cout << client[i].socket << std::endl;
		}

		if (tempID != -1)
		{
			//Send the id to that client
			std::cout << "Client #" << client[tempID].clientID << " Accepted" << std::endl;
			message = std::to_string(client[tempID].clientID);
			send(client[tempID].socket, message.c_str(), strlen(message.c_str()), 0);

			//Create a thread process for that client
			my_thread[tempID] = std::thread(processClient, std::ref(client[tempID]), std::ref(client), std::ref(my_thread[tempID]));
		}
		else
		{
			message = "Server is full";
			send(incoming, message.c_str(), strlen(message.c_str()), 0);
			std::cout << message << std::endl;
		}
	} //end while

	//Close listening socket
	closesocket(serverSocket);

	//Close client socket
	for (int i = 0; i < MAX_CLIENT_AMOUNT; i++)
	{
		my_thread[i].detach();
		closesocket(client[i].socket);
	}

	//Clean up Winsock
	WSACleanup();
	std::cout << "Program has ended successfully" << std::endl;

	system("pause");
	return 0;
}