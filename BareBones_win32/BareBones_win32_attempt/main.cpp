#include <windows.h>
#include <string>
#include <iostream>

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
constexpr auto SETTINGS_CHANGE_TITLE = 4;
constexpr auto BUTTON_SET_TITLE = 5;
constexpr auto CONNECTION_VIEW = 6;

// here we declare a method for the content of the window
void AddControls(HWND);
void AddConnect(HWND);
// the edit style needs a handler so we can get and manipulate user input
HWND hEdit;							// we declare the edit handler

void PopUp(HWND, LPCWSTR, LPCWSTR);					// we also declare a popup method

// the win32 kind of main().. it needs a reference for different things.
// hInst = instance of application, aka id of app
// LPSTR = arguments passed to program from cmdline
// ncmdshow = tells us how to display the window
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) 
{
	
	// we need to define a window class to create a window!
	WNDCLASSW wc = {0};
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
		L"WindowName",						// the name that will be displayed on the window
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
			PopUp(hMainWindow, L"404 Game not found!", L"Warning");			// a popup to replace content....
			break;
		case GAME_OPTION_CLOSE:
			DestroyWindow(hMainWindow);	// close the window
			break;
		case SETTINGS_OPTION_RESIZE:
			MessageBeep(MB_OK);		// make a little beep
			break;
		case SETTINGS_CHANGE_TITLE:
			GetWindowTextW(hEdit, text, 100); // and so we save contents of hEdit to text..
			SetWindowTextW(hMainWindow, text);
			break;
		case BUTTON_SET_TITLE:
			GetWindowTextW(hEdit, text, 100); // and so we save contents of hEdit to text..
			SetWindowTextW(hMainWindow, text);
			break;
		case CONNECTION_VIEW:                       // in here we make the window switch 
			HWND hConnectWindow = CreateWindowW(
				L"MyWindowClass2",					// create it from the class we created
				L"WindowName",						// the name that will be displayed on the window
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
			break;
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
	switch(msg) 
	{
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

	AppendMenu(hSettings, MF_POPUP,(UINT_PTR)hSettingsSubMenu, "Window");
	AppendMenu(hSettingsSubMenu, MF_STRING, SETTINGS_OPTION_RESIZE, "Resize");
	AppendMenu(hSettingsSubMenu, MF_STRING, SETTINGS_CHANGE_TITLE, "Change Window Title");

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

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		10, 110, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		110, 110, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		210, 110, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		10, 210, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		110, 210, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		210, 210, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		10, 310, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		110, 310, 100, 100,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"X/O",
		WS_VISIBLE | WS_CHILD,
		210, 310, 100, 100,
		hWnd,
		NULL,
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

	CreateWindowW(
		L"Edit",
		L"Room chat",
		WS_VISIBLE | WS_CHILD,
		410, 100, 300, 250,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Edit",
		L"Response box",
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
		NULL,
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

	CreateWindowW(
		L"Edit",
		L"ip address ",
		WS_VISIBLE | WS_CHILD,
		175, 40, 150, 20,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Static",
		L"Please write port: ",
		WS_VISIBLE | WS_CHILD,
		15, 70, 200, 200,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Edit",
		L"port nr ",
		WS_VISIBLE | WS_CHILD,
		175, 70, 150, 20,
		hWnd,
		NULL,
		NULL,
		NULL
	);

	CreateWindowW(
		L"Button",
		L"Connect",
		WS_VISIBLE | WS_CHILD,
		15, 100, 150, 30,
		hWnd,
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