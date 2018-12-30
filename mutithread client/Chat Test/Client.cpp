#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")

// In here we a defaul buffer lenght has to be set in order to send messages
#define DEFAULT_BUFLEN 512            

//In here we define an IP address and a port number in order to establish a connection
#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "54000"


//We define a structure that has an int for the id, a SOCKET varaible which is an endpoint,
//of a two-way communication link between two programs running on the network, and a char var called message_recived.
struct client_type
{
	SOCKET sock;
	int id;
	char message_recived[DEFAULT_BUFLEN];
};

int process_client(client_type &new_client);
int main();

//This method has its own thread
int process_client(client_type &new_client)
{
	while (1)
	{
		//Here we set a pointer to the memory to fill (new_client.message_recived), a value that is going to be set in this case 0,
		//and the number of bytes to be set to the value (DEFAULT_BUFLEN)
		memset(new_client.message_recived, 0, DEFAULT_BUFLEN);

		//Here we check if the new client is not 0 
		if (new_client.sock != 0)
		{
			//The Result variable purpose is to be equal to what is recived from the client, a socket, a message and a value.
			int Result = recv(new_client.sock, new_client.message_recived, DEFAULT_BUFLEN, 0);

			//If there are no errors the message from the client will be displayed,
			//otherwise an error meassage will be displayed "Recieved message failed"
			if (Result != SOCKET_ERROR)
				std::cout << new_client.message_recived << std::endl;
			else
			{
				std::cout << "Recieved message failed" << WSAGetLastError() << std::endl;
				break;
			}
		}
	}

	//If the connection was forcibly closed by the remote host the "Discosnected sever" message will be displayed
	if (WSAGetLastError() == WSAECONNRESET)
		std::cout << "Discosnected sever" << std::endl;

	return 0;
}

int main()
{
	//These are the variables that we need for the Winsock protocol
	WSAData wsa_data;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	std::string sent_message = "";
	client_type client = { INVALID_SOCKET, -1, "" };
	int Result = 0;
	std::string message;

	std::cout << "Client starting...\n";

	// Initialize Winsock
	Result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (Result != 0) {
		std::cout << "WSA failed" << Result << std::endl;
		return 1;
	}

	//The ZeroMemory method is used to fill the memory block with 0
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Unspecified adress family, can be either IPv4, IPv6, etc.
	hints.ai_socktype = SOCK_STREAM; // Sock stream for TCP socket
	hints.ai_protocol = IPPROTO_TCP; // Use of TCP protocol

	std::cout << "Connecting \n";

	// Resolve the server address and port
	Result = getaddrinfo(static_cast<LPCTSTR>(IP_ADDRESS), DEFAULT_PORT, &hints, &result);
	if (Result != 0) {
		std::cout << "Get address failed " << Result << std::endl;
		WSACleanup();
		system("On hold");
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Here a SOCKET is created in order to connect to the server
		client.sock = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (client.sock == INVALID_SOCKET) {
			std::cout << "Socket failed " << WSAGetLastError() << std::endl;
			WSACleanup();
			system("On hold");
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

	// In case of an invalid socket a error meassge will be shown
	if (client.sock == INVALID_SOCKET) {
		std::cout << "Cannot connect to sever get lost!" << std::endl;
		WSACleanup();
		system("On hold");
		return 1;
	}
	//If client connected to server a confirmation message will be displayed
	std::cout << " Connected:)" << std::endl;

	//In here the id from server for this client is obtained
	recv(client.sock, client.message_recived, DEFAULT_BUFLEN, 0);
	message = client.message_recived;

	if (message != "Server has no more room, get lost!")
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
				std::cout << "Message  failed: " << WSAGetLastError() << std::endl;
				break;
			}
		}

		//Shutdown the connection since no more data will be sent
		thread.detach();
	}
	else
		std::cout << client.message_recived << std::endl;

	std::cout << "Socket.. shutting down" << std::endl;
	Result = shutdown(client.sock, SD_SEND);
	if (Result == SOCKET_ERROR) {
		std::cout << "Shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(client.sock);
		WSACleanup(); // Terminate use of Winsock 2 DLL
		system("On hold");
		return 1;
	}

	//In here we disconnect by cleaning the Winsock
	closesocket(client.sock);
	WSACleanup();
	system("On hold!");
	return 0;
}