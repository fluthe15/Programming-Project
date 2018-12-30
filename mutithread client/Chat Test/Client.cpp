#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512            
#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "54000"

struct client_type
{
	SOCKET sock;
	int id;
	char message_recived[DEFAULT_BUFLEN];
};

int process_client(client_type &new_client);
int main();

int process_client(client_type &new_client)
{
	while (1)
	{
		memset(new_client.message_recived, 0, DEFAULT_BUFLEN);

		if (new_client.sock != 0)
		{
			int Result = recv(new_client.sock, new_client.message_recived, DEFAULT_BUFLEN, 0);

			if (Result != SOCKET_ERROR)
				std::cout << new_client.message_recived << std::endl;
			else
			{
				std::cout << "recieve message failed: " << WSAGetLastError() << std::endl;
				break;
			}
		}
	}

	if (WSAGetLastError() == WSAECONNRESET)
		std::cout << "discosnected sever" << std::endl;

	return 0;
}

int main()
{
	WSAData wsa_data;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	std::string sent_message = "";
	client_type client = { INVALID_SOCKET, -1, "" };
	int Result = 0;
	std::string message;

	std::cout << "client starting \n";

	// Initialize Winsock
	Result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (Result != 0) {
		std::cout << "WSA failed" << Result << std::endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Unspecified adress family, can be either IPv4, IPv6, etc.
	hints.ai_socktype = SOCK_STREAM; // Sock stream for TCP socket
	hints.ai_protocol = IPPROTO_TCP; // Use of TCP protocol

	std::cout << "Connecting \n";

	// Resolve the server address and port
	Result = getaddrinfo(static_cast<LPCTSTR>(IP_ADDRESS), DEFAULT_PORT, &hints, &result);
	if (Result != 0) {
		std::cout << "getadress failed " << Result << std::endl;
		WSACleanup();
		system("on hold");
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		client.sock = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (client.sock == INVALID_SOCKET) {
			std::cout << "socket failed " << WSAGetLastError() << std::endl;
			WSACleanup();
			system("on hold");
			return 1;
		}

		// Connect to server.
		Result = connect(client.sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (Result == SOCKET_ERROR) {
			closesocket(client.sock);
			client.sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	// Display error message if invalid socket
	if (client.sock == INVALID_SOCKET) {
		std::cout << "cannot connect to sever get lost!" << std::endl;
		WSACleanup();
		system("on hold");
		return 1;
	}
	// Display message if client connected to server
	std::cout << " Connected :)" << std::endl;

	//Obtain id from server for this client;
	recv(client.sock, client.message_recived, DEFAULT_BUFLEN, 0);
	message = client.message_recived;

	if (message != "Server has no more room get lost!")
	{
		client.id = atoi(client.message_recived);

		std::thread thread(process_client, std::ref(client));

		while (1)
		{
			std::getline(std::cin, sent_message);
			if (sent_message != "") 
			{
				Result = send(client.sock, sent_message.c_str(), strlen(sent_message.c_str()), 0);
			}
		

			if (Result <= 0)
			{
				std::cout << "message  failed: " << WSAGetLastError() << std::endl;
				break;
			}
		}

		//Shutdown the connection since no more data will be sent
		thread.detach();
	}
	else
		std::cout << client.message_recived << std::endl;

	std::cout << "socket.. shutting down" << std::endl;
	Result = shutdown(client.sock, SD_SEND);
	if (Result == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(client.sock);
		WSACleanup(); // Terminate use of Winsock 2 DLL
		system("on hold");
		return 1;
	}

	// Graceful disconnect
	closesocket(client.sock);
	WSACleanup();
	system("on hold!");
	return 0;
}