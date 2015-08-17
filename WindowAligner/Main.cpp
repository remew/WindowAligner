#include <Windows.h>

#define WM_TASKTRAY (WM_USER + 1)
#define ID_TASKTRAY (0)
#define ID_HOTKEY_LEFT (101)
#define ID_HOTKEY_RIGHT (102)
#define ID_HOTKEY_UP (103)
#define ID_HOTKEY_DOWN (104)

#define DIR_LEFT (1)
#define DIR_RIGHT (2)
#define DIR_UP (3)
#define DIR_DOWN (4)

HWND hWnd;
HMENU hMenuP;

bool inLeft(int width, int height, RECT* rect)
{
	return rect->right <= width / 2;
}

bool inRight(int width, int height, RECT* rect)
{
	return rect->left > width / 2;
}

bool inTop(int width, int height, RECT* rect)
{
	return rect->bottom <= height / 2;
}

bool inBottom(int width, int height, RECT* rect)
{
	return rect->top > height / 2;
}

void goLeftTop(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, 0, 0, width / 2, height / 2, 0);
}

void goLeftBottom(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, 0, height / 2 + 1, width / 2, height / 2, 0);
}

void goRightTop(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, width / 2 + 1, 0, width / 2, height / 2, 0);
}

void goRightBottom(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, width / 2 + 1, height / 2 + 1, width / 2, height / 2, 0);
}

void goLeft(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, 0, 0, width / 2, height, 0);
}

void goRight(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, width / 2 + 1, 0, width / 2, height, 0);
}

void goTop(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, 0, 0, width, height / 2, 0);
}

void goBottom(HWND hWnd, int width, int height)
{
	SetWindowPos(hWnd, NULL, 0, height / 2 + 1, width, height / 2, 0);
}

void MoveWindow(int dir)
{
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd = GetForegroundWindow(); //最前面のウィンドウのハンドルを取得
	RECT rect;
	GetWindowRect(hWnd, &rect); //ウィンドウの位置とサイズ
	switch ( dir )
	{
		case DIR_LEFT:
			if ( inTop(width, height, &rect) )
			{
				goLeftTop(hWnd, width, height);
			}
			else if ( inBottom(width, height, &rect) )
			{
				goLeftBottom(hWnd, width, height);
			}
			else
			{
				goLeft(hWnd, width, height);
			}
			break;
		case DIR_RIGHT:
			if ( inTop(width, height, &rect) )
			{
				goRightTop(hWnd, width, height);
			}
			else if ( inBottom(width, height, &rect) )
			{
				goRightBottom(hWnd, width, height);
			}
			else
			{
				goRight(hWnd, width, height);
			}
			break;
		case DIR_UP:
			if ( inLeft(width, height, &rect) )
			{
				goLeftTop(hWnd, width, height);
			}
			else if ( inRight(width, height, &rect) )
			{
				goRightTop(hWnd, width, height);
			}
			else
			{
				goTop(hWnd, width, height);
			}
			break;
		case DIR_DOWN:
			if ( inLeft(width, height, &rect) )
			{
				goLeftBottom(hWnd, width, height);
			}
			else if ( inRight(width, height, &rect) )
			{
				goRightBottom(hWnd, width, height);
			}
			else
			{
				goBottom(hWnd, width, height);
			}
			break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_CREATE:
			hMenuP = CreatePopupMenu();
			AppendMenu(hMenuP, MF_STRING, 1, TEXT("Exit"));
			break;
		case WM_HOTKEY: //ホットキーのイベント
			switch ( wParam )
			{
				case ID_HOTKEY_LEFT:
					MoveWindow(DIR_LEFT);
					break;
				case ID_HOTKEY_RIGHT:
					MoveWindow(DIR_RIGHT);
					break;
				case ID_HOTKEY_UP:
					MoveWindow(DIR_UP);
					break;
				case ID_HOTKEY_DOWN:
					MoveWindow(DIR_DOWN);
					break;
			}
			break;
		case WM_COMMAND: //コンテキストメニューのイベント
			switch ( LOWORD(wParam) )
			{
				case 1:
					DestroyMenu(hMenuP);
					DestroyWindow(hWnd);
					break;
			}
			break;
		case WM_TASKTRAY: //タスクトレイイベント
			if ( wParam == ID_TASKTRAY )
			{
				switch ( lParam ) //マウスイベント
				{
					case WM_RBUTTONUP: //右クリックならコンテキストメニューを出す
						POINT pt;
						GetCursorPos(&pt);
						TrackPopupMenu(hMenuP, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
						break;
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL Init(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, TEXT("WA_ICON"));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("Window Aligner");
	if ( !RegisterClass(&wc) )return EXIT_FAILURE;

	hWnd = CreateWindow(
		TEXT("Window Aligner"),	//class
		TEXT("Window Aligner"),	//title
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, //style
		0,		//x
		0,		//y
		100,	//width
		100,	//height
		NULL,	//menu handle
		NULL,	//instance handle
		hInstance,
		NULL
	);
	if ( !hWnd ) return EXIT_FAILURE;
	//ShowWindow(hWnd, SW_SHOW); //表示する関数 常駐型なので表示しない
	//UpdateWindow(hWnd);

	NOTIFYICONDATA nif;
	nif.cbSize = sizeof(NOTIFYICONDATA);
	nif.hIcon = LoadIcon(hInstance, TEXT("WA_ICON"));
	nif.hWnd = hWnd;
	nif.uCallbackMessage = WM_TASKTRAY;
	nif.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nif.uID = ID_TASKTRAY;
	wcscpy_s(nif.szTip, 128, TEXT("Window Aligner"));
	Shell_NotifyIcon(NIM_ADD, &nif);

	RegisterHotKey(hWnd, ID_HOTKEY_LEFT, MOD_ALT | MOD_SHIFT, VK_LEFT);
	RegisterHotKey(hWnd, ID_HOTKEY_RIGHT, MOD_ALT | MOD_SHIFT, VK_RIGHT);
	RegisterHotKey(hWnd, ID_HOTKEY_UP, MOD_ALT | MOD_SHIFT, VK_UP);
	RegisterHotKey(hWnd, ID_HOTKEY_DOWN, MOD_ALT | MOD_SHIFT, VK_DOWN);
	return EXIT_SUCCESS;
}

void End()
{
	NOTIFYICONDATA nif;
	nif.cbSize = sizeof(NOTIFYICONDATA);
	nif.hWnd = hWnd;
	nif.uID = ID_TASKTRAY;
	Shell_NotifyIcon(NIM_DELETE, &nif);

	UnregisterHotKey(hWnd, ID_HOTKEY_LEFT);
	UnregisterHotKey(hWnd, ID_HOTKEY_RIGHT);
	UnregisterHotKey(hWnd, ID_HOTKEY_UP);
	UnregisterHotKey(hWnd, ID_HOTKEY_DOWN);
}

//BOOL monitorEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*INT count = 0;
	TCHAR szTitle[1024];
	TCHAR szClass[1024];
	TCHAR message[4096];
	//EnumWindows(EnumWindowsProc, (LPARAM)&count);
	HWND hWnd = GetForegroundWindow(); //最前面のウィンドウのハンドルを取得
	RECT rect;
	//GetWindowRect(hWnd, &rect); //ウィンドウの位置とサイズ
	//GetWindowText(hWnd, szTitle, sizeof(szTitle));
	//GetClassName(hWnd, szClass, sizeof(szClass));
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(hWnd, NULL, 0, 0, width / 2, height / 2, 0);
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)monitorEnumProc, 0);
	wsprintf(message, TEXT("%d %d"), width, height);
	MessageBox(NULL, message, TEXT("hoge"), MB_OK);*/
	MSG msg;
	if ( Init(hInstance) == EXIT_FAILURE ) return EXIT_FAILURE;

	while ( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	End();

	return EXIT_SUCCESS;
}

/*BOOL monitorEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData)
{
	monitors[monitors_max].bottom = lpMon->bottom;
	monitors[monitors_max].left = lpMon->left;
	monitors[monitors_max].right = lpMon->right;
	monitors[monitors_max].top = lpMon->top;
	if ( monitors_max >= MONITORS_MAX_LIM - 1 )
	{
		return FALSE;
	}
	monitors_max++;
	return TRUE;
}*/
