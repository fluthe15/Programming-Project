#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

// Definening the Ip address and the port number to connect to
#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "54000"

// Setting up the default buffer lenght for the messages sent
#define DEFAULT_BUFLEN 512

// Setting up a data strcture with the name clientType containing an integer for client 
// identification and a type refference for socket.
struct clientType
{
	int clientID;
	SOCKET socket;
};

// Setting up constants because they dont need to be changed
const char OPTION_VALUE = 1;
const int MAX_CLIENT_AMOUNT = 5;

// Method processClient are declared
int processClient(clientType &newClient, std::vector<clientType> &clientArray, std::thread &thread);
int main();

// comment here!
// This is the method called processClient, which is called in its own seperate thread
int processClient(clientType &newClient, std::vector<clientType> &clientArray, std::thread &thread)
{
	//A string named message and a char named tempMessage with the lenght of the default buffer
	//is both declared and initialized.
	std::string message = "";
	char tempMessage[DEFAULT_BUFLEN] = "";


	while (1)
	{
		//Sets the first number of bytes (DEFAULT_BUFLEN) of the block of memory pointed by 
		//ptr (tempMessage) to the specified value (0)
		memset(tempMessage, 0, DEFAULT_BUFLEN);

		//Checks if the new client is not equal to 0.
		if (newClient.socket != 0)
		{
			//Initialization of the integer result. It is set to be equal to what is recived from the new client
			//such as socket, a message, the default lenght and a value.
			int result = recv(newClient.socket, tempMessage, DEFAULT_BUFLEN, 0);

			//Checks if an error had occoured, if not then it proceeds to take message recieved
			//and then display it with client number and what the content of the message is.
			if (result != SOCKET_ERROR)
			{
				if (strcmp("", tempMessage))
					message = "Client #" + std::to_string(newClient.clientID) + ": " + tempMessage;
				
				//Here it outputs the message to the server console
				std::cout << message.c_str() << std::endl;

				//here it takes the message and the client number and send it to all other clients that is not
				//the original sender.
				for (int i = 0 ; i < MAX_CLIENT_AMOUNT ; i++)
				{
					if (clientArray[i].socket != INVALID_SOCKET)
						if (newClient.clientID != i)
							result = send(clientArray[i].socket, message.c_str(), strlen(message.c_str()), 0);
				}
			}

			//If error orcoured then show client is disconnected.
			else
			{
				message = "Client #" + std::to_string(newClient.clientID) + " Disconnected";

				//Here the message about disconnection is written to the server console
				std::cout << message << std::endl;

				//The sockets for that client is then closed and the socket is set to be invalid
				closesocket(newClient.socket);
				closesocket(clientArray[newClient.clientID].socket);
				clientArray[newClient.clientID].socket = INVALID_SOCKET;

				//If the socket is not invalid send a message
				for (int i = 0; i < MAX_CLIENT_AMOUNT; i++)
				{
					if (clientArray[i].socket != INVALID_SOCKET)
						result = send(clientArray[i].socket, message.c_str(), strlen(message.c_str()), 0);
				}

				break;
			}
		}
	}
	//When the while loop is broken the thread is then detached.
	thread.detach();

	return 0;
}

int main()
{

	// variables needed for the winsock protocol
	WSADATA wsaData;
	struct addrinfo hints;
	struct addrinfo *server = NULL;
	SOCKET serverSocket = INVALID_SOCKET;
	std::string message = "";
	std::vector<clientType> client(MAX_CLIENT_AMOUNT);
	int numberOfClients = 0;
	int tempID = -1;
	std::thread my_thread[MAX_CLIENT_AMOUNT];

	// Here the Winsock is initialized and displays a message in the server confirming it
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
		// we do some resetting, accept the connection to the server, and do some error checking
		SOCKET incoming = INVALID_SOCKET;
		incoming = accept(serverSocket, NULL, NULL);

		if (incoming == INVALID_SOCKET) continue;

		//Reset the number of clients
		numberOfClients = -1;

		//Create a temporary id for the next client so we can tell them apart and use it as a prefix
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
			//Send the id to that client, and show it as accepted in server
			std::cout << "Client #" << client[tempID].clientID << " Accepted" << std::endl;
			message = std::to_string(client[tempID].clientID);
			send(client[tempID].socket, message.c_str(), strlen(message.c_str()), 0);

			//Create a thread process for that client
			my_thread[tempID] = std::thread(processClient, std::ref(client[tempID]), std::ref(client), std::ref(my_thread[tempID]));
		}
		else
		{
			// in case we are full up on clients, send a message to the connectee.
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