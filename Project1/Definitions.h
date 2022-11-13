#pragma once



#define BINDING_STYLE		WS_VISIBLE | WS_CHILD  

#define ON_MENU_CLICK_1	    1
#define ON_MENU_CLICK_2	    2
#define ON_MENU_CLICK_3	    3

#define ON_BUTTON_CLICK_1	  4
#define ON_BUTTON_CLICK_2	  5
#define ON_BUTTON_CLICK_3  	6

#define ON_CONNECTED        7
#define ON_SERIAL_REFRESH   8

#define TEXT_BUFFER_SIZE    256
#define COM_SELECT_INDEX    120
#define COM_PORT_AMOINT     50


char Buffer[TEXT_BUFFER_SIZE];

HWND Lable1;
HWND Lable2;
HWND TextBox1;
HWND TextBox2;

HMENU ComPortSubMenu;
HMENU ComPortListMenu;


char fileName[260];
OPENFILENAMEA ofn;

bool isConnected = false;
int selectedPort = 3;
int targetBaudRate = 9600;
HANDLE connectedPort;

LRESULT CALLBACK MainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowsClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);
void MainWindAddMenus(HWND hWnd);
void MainWindAddWidgets(HWND hWnd);
void SaveData(LPCSTR path, HWND TextBox);
void LoadData(LPCSTR path, HWND TextBox);
void SetOpenFileParams(HWND hWnd);
void SetWindowStatus(std::string status);
