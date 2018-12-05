#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <sstream>

#define SCK_VERSION 0x0202

using namespace std;

void board();

char square[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int main()
{
    SOCKET sock;
    SOCKADDR_IN address;
    char mark = 'X';

    long ok;
    char MESSAGE[200];

    WSAData WSD;
    WORD DllVersion;
    DllVersion = MAKEWORD(2,1);
    ok = WSAStartup(DllVersion, &WSD);

    while(true)
    {
       sock = socket(AF_INET, SOCK_STREAM, NULL);

       address.sin_addr.s_addr = inet_addr("127.0.0.1");
       address.sin_family = AF_INET;
       address.sin_port = htons(10102);

       connect(sock, (SOCKADDR*)&address, sizeof(address));
       board();

       string msg;
       cout<<"You are player 1, please enter a number:"<<endl;
       cin>>msg;

       if (msg == "1" && square[1] == '1')
       square[1] = mark;
       else if (msg == "2" && square[2] == '2')
       square[2] = mark;
       else if (msg == "3" && square[3] == '3')
       square[3] = mark;
       else if (msg == "4" && square[4] == '4')
       square[4] = mark;
       else if (msg == "5" && square[5] == '5')
       square[5] = mark;
       else if (msg == "6" && square[6] == '6')
       square[6] = mark;
       else if (msg == "7" && square[7] == '7')
       square[7] = mark;
       else if (msg == "8" && square[8] == '8')
       square[8] = mark;
       else if (msg == "9" && square[9] == '9')
       square[9] = mark;
       else
       cout<<"Invalid move ";

       const char* s = msg.c_str();
       ok = send(sock, s, 1024,NULL);
       ok = recv(sock, MESSAGE, sizeof(MESSAGE), NULL);
       //string reply;
       //reply = MESSAGE;
       //cout<<"Server says:\t"<<reply<<endl;
    }
}

void board()
{
	cout << "Welcome to Tic Tac Toe!";
	cout << endl;
	cout << "     |     |     " << endl;
	cout << "  " << square[1] << "  |  " << square[2] << "  |  " << square[3] << endl;
	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;
	cout << "  " << square[4] << "  |  " << square[5] << "  |  " << square[6] << endl;
	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;
	cout << "  " << square[7] << "  |  " << square[8] << "  |  " << square[9] << endl;
	cout << "     |     |     " << endl << endl;
}
