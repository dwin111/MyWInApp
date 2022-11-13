#include <Windows.h>
#include <string>
#include "Definitions.h"
#include "COMComunication.h"

/*

,--.   ,--. ,--.   ,--. ,--.   ,--. ,--. ,--.  ,--.   ,---.   ,------.  ,------.
|   `.'   |  \  `.'  /  |  |   |  | |  | |  ,'.|  |  /  O  \  |  .--. ' |  .--. '
|  |'.'|  |   '.    /   |  |.'.|  | |  | |  |' '  | |  .-.  | |  '--' | |  '--' |
|  |   |  |     |  |    |   ,'.   | |  | |  | `   | |  | |  | |  | --'  |  | --'
`--'   `--'     `--'    '--'   '--' `--' `--'  `--' `--' `--' `--'      `--'

Font Name: Soft

*/

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR argd, int ncmdshow)
{
	WNDCLASS MainClass = NewWindowsClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_APPLICATION),
		L"MainWinClass", MainProcedure);

	if (!RegisterClassW(&MainClass))
	{
		return -1;
	}

	MSG MainMessage = { 0 };

	CreateWindow(L"MainWinClass", L"First Window", (WS_OVERLAPPEDWINDOW | WS_VISIBLE), 100, 100, 500, 250, NULL, NULL, NULL, NULL);

	while (GetMessage(&MainMessage, NULL, NULL, NULL))
	{
		TranslateMessage(&MainMessage);
		DispatchMessage(&MainMessage);

	}
	return 0;
}

WNDCLASS NewWindowsClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure)
{
	WNDCLASS NWC = { 0 };

	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK MainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (isConnected) {
		SerialRead();
	}
	switch (msg){
	case WM_CREATE:
		MainWindAddMenus(hWnd);
		MainWindAddWidgets(hWnd);
		SetOpenFileParams(hWnd);
		break;
	case WM_COMMAND:
		switch (wp)
		{
		case ON_MENU_CLICK_1:
			MessageBoxA(hWnd, "THIS IS File", "THIS IS File", MB_OK);
			break;
		case ON_MENU_CLICK_2:
			MessageBoxA(hWnd, "THIS IS Window", "THIS IS Window", MB_OK);
			break;
		case ON_MENU_CLICK_3:
			MessageBoxA(hWnd, "THIS IS View", "THIS IS View", MB_OK);
			break;
		case ON_BUTTON_CLICK_1:
			SetWindowTextA(TextBox1, "");
			SetWindowTextA(TextBox2, "");
			break;
		case ON_BUTTON_CLICK_2:
			if (GetOpenFileNameA(&ofn)) {
				LoadData(fileName, TextBox2);
			}		
			break;
		case ON_BUTTON_CLICK_3:
			if (GetSaveFileNameA(&ofn)) {
				SaveData(fileName, TextBox2);
			}
			break;
		case ON_CONNECTED:
			ConnectRequest();
			break;
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
		break;

	}
}

void MainWindAddMenus(HWND hWnd)
{
	HMENU RootMenu = CreateMenu(); //�������� TollBar
	HMENU SubMenu = CreateMenu(); //�������� TollBar

	AppendMenu(SubMenu, MF_STRING, ON_BUTTON_CLICK_3, L"Save");
	AppendMenu(SubMenu, MF_STRING, ON_BUTTON_CLICK_2, L"Load");
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);               //-------------------
	AppendMenu(SubMenu, MF_STRING, ON_CONNECTED, L"Connected");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");

	SetMenu(hWnd, RootMenu);
}

void MainWindAddWidgets(HWND hWnd)
{
	Lable1 = CreateWindowA("static", "Hellow", BINDING_STYLE | ES_CENTER, 5, 5, 490, 20, hWnd, NULL, NULL, NULL); // LABLE

	Lable2 = CreateWindowA("static", "Hellow", BINDING_STYLE | ES_CENTER, 5, 150, 490, 20, hWnd, NULL, NULL, NULL); // LABLE

	TextBox1 = CreateWindowA("edit", "Hellow", BINDING_STYLE | ES_MULTILINE | WS_VSCROLL, 5, 50, 490, 100, hWnd, NULL, NULL, NULL); // TEXTBOX

	TextBox2 = CreateWindowA("edit", "Hellow", BINDING_STYLE | ES_MULTILINE | WS_VSCROLL, 5, 200, 490, 100, hWnd, (HMENU)200, NULL, NULL); // TEXTBOX

	CreateWindowA("button", "Hellow", BINDING_STYLE | ES_CENTER, 5, 20, 490, 30, hWnd, (HMENU)ON_BUTTON_CLICK_1, NULL, NULL); // BUTTON

	CreateWindowA("button", "Read", BINDING_STYLE | ES_CENTER, 500, 200, 50, 30, hWnd, (HMENU)ON_BUTTON_CLICK_2, NULL, NULL); // BUTTON

	CreateWindowA("button", "Save", BINDING_STYLE | ES_CENTER, 1000, 200, 50, 30, hWnd, (HMENU)ON_BUTTON_CLICK_3, NULL, NULL); // BUTTON
}

void SaveData(LPCSTR path, HWND TextBox)
{
	HANDLE FileToSave = CreateFileA(
		path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	int saveLength = GetWindowTextLength(TextBox);

	char* data = new char[saveLength + 1];

	saveLength = GetWindowTextA(TextBox, data, saveLength + 1);

	DWORD bytesInterated;
	WriteFile(FileToSave, data, saveLength, &bytesInterated, NULL);

	CloseHandle(FileToSave);

	delete[] data;
}

void LoadData(LPCSTR path, HWND TextBox)
{
	HANDLE FileToLoad = CreateFileA(
		path,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	DWORD bytesInterated;
	bool InRead = ReadFile(FileToLoad, Buffer, TEXT_BUFFER_SIZE, &bytesInterated, NULL);

	SetWindowTextA(TextBox, Buffer);
	CloseHandle(FileToLoad);

}

void SetOpenFileParams(HWND hWnd)
{
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = sizeof(fileName);
	ofn.lpstrFilter = "*.txt";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = "D:\\";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void SetWindowStatus(std::string status)
{
	SetWindowTextA(Lable1, status.c_str());
}
