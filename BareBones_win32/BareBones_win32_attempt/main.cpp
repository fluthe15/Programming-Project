#include "pch.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <WS2tcpip.h>
#include <ws2def.h>
#include <WinSock2.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#pragma comment(lib, "ws2_32.lib")


using namespace std;

string IPADD;
int PORT;
string USERNAME;

// variables to control size and placement of the window
int winWidth, winHeight, winPosX, winPosY, monWidth, monHeight;



// this method is used by the WndProc (window procedure) that handles messages
LRESULT CALLBACK WinProcedure(HWND, UINT, WPARAM, LPARAM);

// here we declare a method for the menuline in the top of the window plus some things related to it
void AddMenus(HWND);
HMENU hMenu;						// we declare a HEADERMENU (menuline)

// define the choices as constants for easy reading
constexpr auto GAME_OPTION_START = 1;
constexpr auto GAME_OPTION_CLOSE = 2;
constexpr auto SETTINGS_OPTION_RESIZE = 3;
constexpr auto SETTINGS_CHANGE_TITLE = 4;
constexpr auto BUTTON_CONNECT = 5;

// here we declare a method for the content of the window
void AddControls(HWND);
// the edit style needs a handler so we can get and manipulate user input
HWND hEdit1;							// we declare the edit handler
HWND hEdit2;
HWND hEdit3;


void PopUp(HWND, LPCWSTR, LPCWSTR);					// we also declare a popup method

// the win32 kind of main().. it needs a reference for different things.
// hInst = instance of application, aka id of app
// LPSTR = arguments passed to program from cmdline
// ncmdshow = tells us how to display the window

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{

	// we need to define a window class to create a window!
	WNDCLASSW wc = { 0 };
	// then we need to fill out this struct with data:
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // we will give background default window color
	// we also need a cursor in the window, we will just use the default arrow!
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// we are given the instance in the WinMain, so we assign it here:
	wc.hInstance = hInst;
	// we also need a class name for this struct (the ident for our class)
	// L is a prefix needed because it does not accept a regular string here
	// it needs LPCWSTR, so it is casted to that with the L
	wc.lpszClassName = L"myWindowClass";
	// we also need something called a "window procedure" part of the event driven system
	// basically all button press-messages etc are sent through this, to make sure everything
	// happens the way we expect it to, in a window. We need a method for this, it's found above
	wc.lpfnWndProc = WinProcedure;

	// now we need to register the class we defined above!
	// we do a quick early-out check to see if we can register
	if (!RegisterClassW(&wc)) { return -1; }

	// we will assign the values for position and size of the window:
	monHeight = 1080; //GetSystemMetrics(SM_CXSCREEN);
	monWidth = 1920; //GetSystemMetrics(SM_CYSCREEN);   <- TODO: these don't give the desired result, solution pending! 

	winWidth = 800;
	winHeight = 550;
	winPosX = monWidth / 2 - winWidth / 2;
	winPosY = monHeight / 2 - winHeight / 2;

	// and then we move on to creating a window:
	CreateWindowW(
		L"MyWindowClass",					// create it from the class we created
		L"Tic-Chat-Toe",						// the name that will be displayed on the window
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 	// this is the style of the window via a constant
		winPosX,								// position of window (X)
		winPosY,								// position of window (Y)
		winWidth,								// size of window (X)
		winHeight,								// size of window (Y)
		NULL,								// parent of window, we have none so NULL
		NULL,								// hMenu is also NULL, its not a menu...
		NULL,								// the hInstance, we pass NULL because the instance is set above
		NULL								// lParams, we pass NULL (ignore it)

		// we now created a window, but it will not show without a loop that shows it!

	);



	// the loop responsible for continually drawing our window
	// this loop will break either when we press the button or otherwise close the window
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		// translate the message at the adress of msg
		TranslateMessage(&msg);
		// send the message at the adress of msg to winProcedure
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WinProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// switch case to see the value of msg
	switch (msg)
	{
		// this is called every time a menu line item or button has been clicked
	case WM_COMMAND:
		// here we use wp(WPARAM) to identify what was clicked
		switch (wp)
		{
			wchar_t text[100];		// a place to save the input from the user
		case GAME_OPTION_START:
			MessageBeep(MB_OK);		// make a little beep
			PopUp(hWnd, L"404 Game not found!", L"Warning");			// a popup to replace content....
			break;
		case GAME_OPTION_CLOSE:
			DestroyWindow(hWnd);	// close the window
			break;
		case SETTINGS_OPTION_RESIZE:
			MessageBeep(MB_OK);		// make a little beep
			break;
		case SETTINGS_CHANGE_TITLE:
			GetWindowTextW(hEdit1, text, 100); // and so we save contents of hEdit to text..
			SetWindowTextW(hWnd, text);
			break;
		case BUTTON_CONNECT:
			GetWindowTextW(hEdit1, text, 100); // and so we save contents of hEdit1 to text..
			GetWindowTextW(hEdit2, text, 100); // and so we save contents of hEdit2 to text..
			GetWindowTextW(hEdit3, text, 100); // and so we save contents of hEdit3 to text..

			Connect_To_Server();
			PopUp(hWnd, L"404 Game not found!", L"Warning");
			break;
		}
		break;

		// the message we expect here is create (called when the window is first created)
	case WM_CREATE:
		AddMenus(hWnd);								// we add the top menuline here
		AddControls(hWnd);
		break;
		// the message we expect here is destroy (close window with x)
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// the default operation for all other messages
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);

	}
}

// this is the method that gives us the top menuline..
void AddMenus(HWND hWnd)
{
	hMenu = CreateMenu();	// here we actually assign hMenu to be a new menu
	// here we create the contents of the menuline
	HMENU hGameMenu = CreateMenu();
	HMENU hSettings = CreateMenu();
	HMENU hSettingsSubMenu = CreateMenu();
	// and fill them out
	AppendMenu(hGameMenu, MF_STRING, GAME_OPTION_START, "Start");
	AppendMenu(hGameMenu, MFT_SEPARATOR, NULL, NULL);
	AppendMenu(hGameMenu, MF_STRING, GAME_OPTION_CLOSE, "Close");

	AppendMenu(hSettings, MF_POPUP, (UINT_PTR)hSettingsSubMenu, "Window");
	AppendMenu(hSettingsSubMenu, MF_STRING, SETTINGS_OPTION_RESIZE, "Resize");
	AppendMenu(hSettingsSubMenu, MF_STRING, SETTINGS_CHANGE_TITLE, "Change Window Title");
	// now that we have a menuline and some content, we need to add the content
	// we use AppendMenu on hMenu, we call MF_POPUP because we want it to show a popup list
	// we also specify the ID of the menu, as a pointer
	// finally we call the new menu option by name.
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, "Game");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSettings, "Settings");


	SetMenu(hWnd, hMenu);	// here we assign it to the window

}

// this is the method that adds content in the window (they are called controls)

void AddControls(HWND hWnd)
{

	int swWidth = 200;
	int swHeight = 100;
	int offset = 5;
	// first we create a window to contain the control
	// we initiate it much like the main window, but we use SS_CENTER (centering text via "static style")
	// and WS_CHILD because it is a child of the main window..
	CreateWindowW(
		L"static",
		L"Welcome to TIC-CHAT-TOE!",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		winWidth / 2 - swWidth / 2,
		winHeight / 2 - swWidth / 2,
		swWidth,
		swHeight,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	// we can do the same, but using edit style, so it can react to user input..
	// that's why we save this one as a variable as opposed to the others!
	hEdit1 = CreateWindowW(
		L"Edit",
		L"Enter Username Here",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		winWidth / 2 - (swWidth + 100) / 2,
		winHeight / 2 - ((swHeight + 100) / 2) + (offset * 5),
		swWidth + 100,
		swHeight - 78,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	//  | ES_MULTILINE

	hEdit2 = CreateWindowW(
		L"Edit",
		L"Enter IP Adress Here",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		winWidth / 2 - (swWidth + 100) / 2,
		winHeight / 2 - ((swHeight + 30) / 2) + (offset * 5),
		swWidth + 100,
		swHeight - 78,
		hWnd,
		NULL,
		NULL,
		NULL

	);


	hEdit3 = CreateWindowW(
		L"Edit",
		L"Enter Port Here",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		winWidth / 2 - (swWidth + 100) / 2,
		winHeight / 2 - ((swHeight - 40) / 2) + (offset * 5),
		swWidth + 100,
		swHeight - 78,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"Connect to Play",
		WS_VISIBLE | WS_CHILD,
		winWidth / 2 - (swWidth) / 2,
		winHeight / 2 - ((swHeight) / 2) + (offset * 32),
		swWidth,
		swHeight - 50,
		hWnd,
		(HMENU)BUTTON_CONNECT,
		NULL,
		NULL,
		NULL
	);

}

// this is the method that makes a popup!
void PopUp(HWND hWnd, LPCWSTR _text, LPCWSTR _name)
{
	LPCWSTR text = _text;
	LPCWSTR name = _name;
	// we create a message-box as a test, MB_OK is the template, 
	MessageBoxW(hWnd, text, name, MB_OK);
}


void Connect_To_Server(string USERNAME, string IPADD, int PORT)
{
	string ipAddress = IPADD;			// IP Address of the server
	int port = PORT;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
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
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//here and up is connection to server

		// Do-while loop to send and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Prompt the user for some text
		cout << "> ";
		getline(cin, userInput);

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
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}


/*

	//here and up is connection to server

// Do-while loop to send and receive data
char buf[4096];
string userInput;

do
{
	// Prompt the user for some text
	cout << "> ";
	getline(cin, userInput);

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
				cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
			}
		}
	}

} while (userInput.size() > 0);

// Gracefully close down everything
closesocket(sock);
WSACleanup();
}

*/


/*
void Client()
{

	//here and up is connection to server

	// Do-while loop to send and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Prompt the user for some text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)		// Make sure the user has typed in something
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
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}
*/