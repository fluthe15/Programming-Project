//#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Client.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
//#include <ws2def.h>
//#include <winsock2.h>
//#include <ws2ipdef.h>
//#pragma comment(lib, "ws2_32.lib")

			using namespace std;

			string IPADD;
			int PORT;
			string USERNAME;
			int gwtstat = 0;
			char textSaved1[20];
			char textSaved2[20];
			char textSaved3[20];

			// Char array to store content of buttons
			char XO1[3]; char XO2[3]; char XO3[3]; char XO4[3]; char XO5[3]; char XO6[3]; char XO7[3]; char XO8[3]; char XO9[3];

			// variables to control size and placement of the window
			int winWidth, winHeight, winPosX, winPosY, monWidth, monHeight;

			// this method is used by the WndProc (window procedure) that handles messages
			LRESULT CALLBACK WinProcedure(HWND, UINT, WPARAM, LPARAM);
			LRESULT CALLBACK WinProcedure2(HWND, UINT, WPARAM, LPARAM);

			// here we declare a method for the menuline in the top of the window plus some things related to it
			void AddMenus(HWND);
			HMENU hMenu;						// we declare a HEADERMENU (menuline)

			// define the choices as constants for easy reading
			constexpr auto GAME_OPTION_START = 1;
			constexpr auto GAME_OPTION_CLOSE = 2;
			constexpr auto SETTINGS_OPTION_RESIZE = 3;
			constexpr auto CONNECTION_VIEW = 4;
			constexpr auto CONNECT_TO_SERVER = 5;
			constexpr auto SEND_BUTTON = 6;
			constexpr auto BTNXO_1 = 7;
			constexpr auto BTNXO_2 = 8;
			constexpr auto BTNXO_3 = 9;
			constexpr auto BTNXO_4 = 10;
			constexpr auto BTNXO_5 = 11;
			constexpr auto BTNXO_6 = 12;
			constexpr auto BTNXO_7 = 13;
			constexpr auto BTNXO_8 = 14;
			constexpr auto BTNXO_9 = 15;


			// here we declare a method for the content of the window
			void AddControls(HWND);
			void AddConnect(HWND);

			// the edit style needs a handler so we can get and manipulate user input
			HWND hEdit;							// we declare the edit handler
			HWND hEdit1;							// we declare the edit handler
			HWND hEdit2;
			HWND hEdit3;
			HWND hWnd;
			HWND chatInput;
			HWND chatOutput;
			HWND userName;
			HWND hBtn1; HWND hBtn2; HWND hBtn3; HWND hBtn4; HWND hBtn5; HWND hBtn6; HWND hBtn7; HWND hBtn8; HWND hBtn9;

			void PopUp(HWND, LPCWSTR, LPCWSTR);					// we also declare a popup method

			CHAR ipAddressString[] = { 0 };
			
			string strOutput;
			string strOutput2;
			string tempString;
			string tempStringTwo;
			string tempStringThree;
			string tempStringFour;
			string tempStringFive;
			string tempStringSix;
			string tempStringSeven;
			string tempStringEight;
			string tempStringNine;
			string A1Buf;
			char chatMSG[4096];
			char charOut[4096];
			char userNameVar[30];
			char tempChar[4096];
			char noTurn[1024] = "Not your turn bucko!";
			

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

				WNDCLASSW wc2 = { 0 };
				// then we need to fill out this struct with data:
				wc2.hbrBackground = (HBRUSH)COLOR_WINDOW; // we will give background default window color
				// we also need a cursor in the window, we will just use the default arrow!
				wc2.hCursor = LoadCursor(NULL, IDC_ARROW);
				// we are given the instance in the WinMain, so we assign it here:
				wc2.hInstance = hInst;
				// we also need a class name for this struct (the ident for our class)
				// L is a prefix needed because it does not accept a regular string here
				// it needs LPCWSTR, so it is casted to that with the L
				wc2.lpszClassName = L"myWindowClass2";
				// we also need something called a "window procedure" part of the event driven system
				// basically all button press-messages etc are sent through this, to make sure everything
				// happens the way we expect it to, in a window. We need a method for this, it's found above
				wc2.lpfnWndProc = WinProcedure2;

				// now we need to register the class we defined above!
				// we do a quick early-out check to see if we can register
				if (!RegisterClassW(&wc)) { return -1; }
				if (!RegisterClassW(&wc2)) { return -1; }

				// we will assign the values for position and size of the window:
				monHeight = 1080; //GetSystemMetrics(SM_CXSCREEN);
				monWidth = 1920; //GetSystemMetrics(SM_CYSCREEN);   <- TODO: these don't give the desired result, solution pending! 

				winWidth = 800;
				winHeight = 550;
				winPosX = monWidth / 2 - winWidth / 2;
				winPosY = monHeight / 2 - winHeight / 2;

				// and then we move on to creating a window:
				HWND hMainWindow = CreateWindowW(
					L"MyWindowClass",					// create it from the class we created
					L"Tic Chat Toe",						// the name that will be displayed on the window
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

			LRESULT CALLBACK WinProcedure(HWND hMainWindow, UINT msg, WPARAM wp, LPARAM lp)
			{
				Client cObject;

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
						PopUp(hMainWindow, L"404 Game not found!", L"Click");			// a popup to replace content....
						break;
					
					case BTNXO_1:
						cObject.sendMessage("1");
						
						tempString = cObject.recvMessage(cObject.buf);

						if (tempString == "OK1")
						{
							SetWindowText(hBtn1, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '1'");
						}
						else if (tempString == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempString == "OK2")
						{
							SetWindowText(hBtn1, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '1'");
						}
						else if (tempString == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						
						/*for (int i = 0; i < tempString.length(); i++)
						{
							tempChar[i] = tempString[i];
						}*/
						
						break;

					case BTNXO_2:
						cObject.sendMessage("2");

						tempStringTwo = cObject.recvMessage(cObject.buf);

						if (tempStringTwo == "OK1")
						{
							SetWindowText(hBtn2, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '2'");
						}
						else if (tempStringTwo == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringTwo == "OK2")
						{
							SetWindowText(hBtn2, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '2'");
						}
						else if (tempStringTwo == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_3:
						cObject.sendMessage("3");

						tempStringThree = cObject.recvMessage(cObject.buf);

						if (tempStringThree == "OK1")
						{
							SetWindowText(hBtn3, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '3'");
						}
						else if (tempStringThree == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringThree == "OK2")
						{
							SetWindowText(hBtn3, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '3'");
						}
						else if (tempStringThree == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_4:
						cObject.sendMessage("4");

						tempStringFour = cObject.recvMessage(cObject.buf);

						if (tempStringFour == "OK1")
						{
							SetWindowText(hBtn4, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '4'");
						}
						else if (tempStringFour == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringFour == "OK2")
						{
							SetWindowText(hBtn4, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '4'");
						}
						else if (tempStringFour == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_5:
						cObject.sendMessage("5");

						tempStringFive = cObject.recvMessage(cObject.buf);

						if (tempStringFive == "OK1")
						{
							SetWindowText(hBtn5, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '5'");
						}
						else if (tempStringFive == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringFive == "OK2")
						{
							SetWindowText(hBtn5, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '5'");
						}
						else if (tempStringFive == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_6:
						cObject.sendMessage("6");

						tempStringSix = cObject.recvMessage(cObject.buf);

						if (tempStringSix == "OK1")
						{
							SetWindowText(hBtn6, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '6'");
						}
						else if (tempStringSix == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringSix == "OK2")
						{
							SetWindowText(hBtn6, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '6'");
						}
						else if (tempStringSix == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_7:
						cObject.sendMessage("7");

						tempStringSeven = cObject.recvMessage(cObject.buf);

						if (tempStringSeven == "OK1")
						{
							SetWindowText(hBtn7, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '7'");
						}
						else if (tempStringSeven == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringSeven == "OK2")
						{
							SetWindowText(hBtn7, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '7'");
						}
						else if (tempStringSeven == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_8:
						cObject.sendMessage("8");

						tempStringEight = cObject.recvMessage(cObject.buf);

						if (tempStringEight == "OK1")
						{
							SetWindowText(hBtn8, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '8'");
						}
						else if (tempStringEight == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringEight == "OK2")
						{
							SetWindowText(hBtn8, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '8'");
						}
						else if (tempStringEight == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case BTNXO_9:
						cObject.sendMessage("9");

						tempStringNine = cObject.recvMessage(cObject.buf);

						if (tempStringNine == "OK1")
						{
							SetWindowText(hBtn9, (LPCSTR) "X");
							SetWindowText(chatOutput, "You placed 'X' on position '9'");
						}
						else if (tempStringNine == "NO1")
						{
							SetWindowText(chatOutput, "Not player 1's turn!");
						}
						else if (tempStringNine == "OK2")
						{
							SetWindowText(hBtn9, (LPCSTR) "O");
							SetWindowText(chatOutput, "You placed 'O' on position '9'");
						}
						else if (tempStringNine == "NO2")
						{
							SetWindowText(chatOutput, "Not player 2's turn!");
						}
						break;

					case GAME_OPTION_CLOSE:
						DestroyWindow(hMainWindow);	// close the window
						break;

					case SETTINGS_OPTION_RESIZE:
						MessageBeep(MB_OK);		// make a little beep
						break;

					case CONNECT_TO_SERVER:
						gwtstat = GetWindowText(hEdit1, &textSaved1[0], 20);
						MessageBox(hWnd, textSaved1, textSaved1, MB_OK);
						break;

					case SEND_BUTTON:
						MessageBeep(MB_ABORTRETRYIGNORE);

						GetWindowText(chatInput, chatMSG, 4096);
						GetWindowText(userName, userNameVar, 30);
						
						

						strcat_s(userNameVar, ": ");
						strcat_s(charOut, userNameVar);
						strcat_s(charOut, chatMSG);
						strcat_s(charOut, "\r\n");
						// send chat message to server
						cObject.sendMessage(charOut);
						// receive chat message from server
						tempString = cObject.recvMessage(cObject.buf);
						//tempString2 = new char[tempString.length()+1];
						//strcpy(tempString2, tempString.c_str);
						for (int i = 0; i < tempString.length(); i++) 
						{
							tempChar[i] = tempString[i];
						}
						//set chat message in chat window
						SetWindowText(chatOutput, tempChar);
						// reset chat input box..
						SetWindowText(chatInput, "");
						break;

					case CONNECTION_VIEW:                       // in here we make the window switch 
						cObject.Connect_To_Server("127.0.0.1", 54000);
						strcat_s(charOut, ">> Connected to server");
						strcat_s(charOut, "\r\n");
						SetWindowText(chatOutput, charOut);

						//HWND hConnectWindow = CreateWindowW(
						//	L"MyWindowClass2",					// create it from the class we created
						//	L"TIC-CHAT-TOE",						// the name that will be displayed on the window
						//	WS_OVERLAPPEDWINDOW | WS_VISIBLE, 	// this is the style of the window via a constant
						//	winPosX,								// position of window (X)
						//	winPosY,								// position of window (Y)
						//	winWidth,								// size of window (X)
						//	winHeight,								// size of window (Y)
						//	NULL,								// parent of window, we have none so NULL
						//	NULL,								// hMenu is also NULL, its not a menu...
						//	NULL,								// the hInstance, we pass NULL because the instance is set above
						//	NULL);								// lParams, we pass NULL (ignore it)

						break;
						// we now created a window, but it will not show without a loop that shows it!

					}
					break;
					// the message we expect here is create (called when the window is first created)
				case WM_CREATE:
					AddMenus(hMainWindow);								// we add the top menuline here
					AddControls(hMainWindow);
					break;
					// the message we expect here is destroy (close window with x)
				case WM_DESTROY:
					PostQuitMessage(0);
					break;
					// the default operation for all other messages
				default:
					return DefWindowProcW(hMainWindow, msg, wp, lp);
				}
			}

			LRESULT CALLBACK WinProcedure2(HWND hConnectWindow, UINT msg, WPARAM wp, LPARAM lp)
			{

				switch (msg)
				{
				case WM_COMMAND:
					switch (wp)
					{
					case CONNECTION_VIEW:
						break;

					default:
						break;
					}
				case WM_CREATE:
					AddMenus(hConnectWindow);								// we add the top menuline here
					AddConnect(hConnectWindow);

					break;
				case WM_DESTROY:
					//PostQuitMessage(0);
					break;

				default:
					return DefWindowProcW(hConnectWindow, msg, wp, lp);
					break;
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
				HMENU hConnectionMenu = CreateMenu();
				// and fill them out
				AppendMenu(hGameMenu, MF_STRING, GAME_OPTION_START, "Start");
				AppendMenu(hGameMenu, MFT_SEPARATOR, NULL, NULL);
				AppendMenu(hGameMenu, MF_STRING, GAME_OPTION_CLOSE, "Close");

				AppendMenu(hSettings, MF_POPUP, (UINT_PTR)hSettingsSubMenu, "Window");
				AppendMenu(hSettingsSubMenu, MF_STRING, SETTINGS_OPTION_RESIZE, "Resize");
				AppendMenu(hConnectionMenu, MF_STRING, CONNECTION_VIEW, "Connect");
				// now that we have a menuline and some content, we need to add the content
				// we use AppendMenu on hMenu, we call MF_POPUP because we want it to show a popup list
				// we also specify the ID of the menu, as a pointer
				// finally we call the new menu option by name.
				AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, "Game");
				AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hConnectionMenu, "Connect");
				AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSettings, "Settings");


				SetMenu(hWnd, hMenu);	// here we assign it to the window

			}

			// this is the method that adds content in the window (they are called controls)
#pragma region
			void AddControls(HWND hWnd)
			{

				CreateWindowW(
					L"Static",
					L"Welcome to Tic Chat Toe!",
					WS_VISIBLE | WS_CHILD | SS_CENTER,
					400, 0, 100, 100,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				hBtn1 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					10, 110, 100, 100,
					hWnd,
					(HMENU) BTNXO_1,
					NULL,
					NULL
				);

				hBtn2 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					110, 110, 100, 100,
					hWnd,
					(HMENU)BTNXO_2,
					NULL,
					NULL
				);

				hBtn3 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					210, 110, 100, 100,
					hWnd,
					(HMENU)BTNXO_3,
					NULL,
					NULL
				);

				hBtn4 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					10, 210, 100, 100,
					hWnd,
					(HMENU)BTNXO_4,
					NULL,
					NULL
				);

				hBtn5 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					110, 210, 100, 100,
					hWnd,
					(HMENU)BTNXO_5,
					NULL,
					NULL
				);

				hBtn6 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					210, 210, 100, 100,
					hWnd,
					(HMENU)BTNXO_6,
					NULL,
					NULL
				);

				hBtn7 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					10, 310, 100, 100,
					hWnd,
					(HMENU)BTNXO_7,
					NULL,
					NULL
				);

				hBtn8 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					110, 310, 100, 100,
					hWnd,
					(HMENU)BTNXO_8,
					NULL,
					NULL
				);

				hBtn9 = CreateWindowW(
					L"Button",
					L"X/O",
					WS_VISIBLE | WS_CHILD,
					210, 310, 100, 100,
					hWnd,
					(HMENU)BTNXO_9,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Static",
					L"Chat room",
					WS_VISIBLE | WS_CHILD,
					410, 80, 75, 75,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				chatOutput = CreateWindowW(
					L"Static",
					(LPCWSTR)charOut,
					WS_VISIBLE | WS_CHILD | SS_EDITCONTROL | WS_BORDER,
					410, 100, 300, 250,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Static",
					L"Username",
					WS_VISIBLE | WS_CHILD,
					10, 20, 300, 20,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				userName = CreateWindowW(
					L"Edit",
					L"Default user",
					WS_VISIBLE | WS_CHILD,
					10, 40, 300, 20,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				chatInput = CreateWindowW(
					L"Edit",
					L"",
					WS_VISIBLE | WS_CHILD,
					410, 360, 300, 80,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Button",
					L"Send",
					WS_VISIBLE | WS_CHILD,
					410, 450, 300, 30,
					hWnd,
					(HMENU)SEND_BUTTON,
					NULL,
					NULL
				);
			}

			void AddConnect(HWND hWnd) {
				CreateWindowW(
					L"Static",
					L"In order to play, please connect with another player!",
					WS_VISIBLE | WS_CHILD,
					15, 15, 400, 450,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Static",
					L"Please write IP address: ",
					WS_VISIBLE | WS_CHILD,
					15, 40, 200, 200,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				hEdit1 = CreateWindowW(
					L"Edit",
					L"Ip address ",
					WS_VISIBLE | WS_CHILD,
					175, 40, 150, 20,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Static",
					L"Please write port nr: ",
					WS_VISIBLE | WS_CHILD,
					15, 70, 200, 200,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				HWND hEdit2 = CreateWindowW(
					L"Edit",
					L"Port nr ",
					WS_VISIBLE | WS_CHILD,
					175, 70, 150, 20,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Static",
					L"Please write username: ",
					WS_VISIBLE | WS_CHILD,
					15, 100, 200, 200,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				HWND hEdit3 = CreateWindowW(
					L"Edit",
					L"Username ",
					WS_VISIBLE | WS_CHILD,
					175, 100, 150, 20,
					hWnd,
					NULL,
					NULL,
					NULL
				);

				CreateWindowW(
					L"Button",
					L"Connect",
					WS_VISIBLE | WS_CHILD,
					15, 130, 150, 30,
					hWnd,
					(HMENU)CONNECTION_VIEW,
					NULL,
					NULL
				);

			}
#pragma endregion AddControls

			// this is the method that makes a popup!
			void PopUp(HWND hWnd, LPCWSTR _text, LPCWSTR _name)
			{
				LPCWSTR text = _text;
				LPCWSTR name = _name;
				// we create a message-box as a test, MB_OK is the template, 
				MessageBoxW(hWnd, text, name, MB_OK);
			}
