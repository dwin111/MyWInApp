#pragma once

#define NO_DEVICES				-4
#define NO_SETTING				-3
#define	FAILED_TO_SET_OPTIONS	-2
#define	FAILED_TO_SET_TIMEOUT	-1
#define	ALL_OK					 0

#define SIZE_SERIAL_BUFFER       11

int SerialBegin(int BaudRate, int comport)
{
	CloseHandle(connectedPort);

	connectedPort = CreateFileA(
		("\\\\.\\COM" + std::to_string(comport)).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (connectedPort == INVALID_HANDLE_VALUE)
	{
		return NO_DEVICES;
	}
	DCB SerialParams;
	SerialParams.DCBlength = sizeof(SerialParams);

	if (!GetCommState(connectedPort, &SerialParams))
	{
		return NO_SETTING;
	}
	SerialParams.BaudRate = BaudRate;
	SerialParams.ByteSize = 8;
	SerialParams.StopBits = ONESTOPBIT;
	SerialParams.Parity = NOPARITY;

	if (!SetCommState(connectedPort, &SerialParams))
	{
		return FAILED_TO_SET_OPTIONS;
	}

	COMMTIMEOUTS SerialTimeouts;
	SerialTimeouts.ReadIntervalTimeout = 1;
	SerialTimeouts.ReadTotalTimeoutConstant = 1;
	SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
	SerialTimeouts.WriteTotalTimeoutConstant = 1;
	SerialTimeouts.WriteTotalTimeoutMultiplier = 1;
	if (!SetCommTimeouts(connectedPort, &SerialTimeouts))
	{
		return FAILED_TO_SET_TIMEOUT;
	}

	return ALL_OK;
}

void ConnectRequest(void)
{
	if (isConnected) {
		CloseHandle(connectedPort);
		SetWindowStatus("Disconnected");
		isConnected = false;
		return;
	}

	switch (SerialBegin(targetBaudRate, selectedPort))
	{
	case NO_DEVICES:
		SetWindowStatus("Device not connected!");
		break;
	case NO_SETTING:
		SetWindowStatus("Device not setting!");
		break;
	case FAILED_TO_SET_OPTIONS:
		SetWindowStatus("SetState error!");
		break;
	case FAILED_TO_SET_TIMEOUT:
		SetWindowStatus("SetTimeouts error!");
		break;
	case ALL_OK:
		SetWindowStatus("Connected tot COM" + std::to_string(selectedPort));
		isConnected = true;
		return;
	}
	CloseHandle(connectedPort);
}

DWORD WINAPI SrialRead(LPVOID lpParameter)
{
	DWORD BytesIterated;

	while (isThreading) {
		if (!isConnected) {
			continue;
		}
		else if (!SetCommMask(connectedPort, EV_RXCHAR)) {
			ConnectRequest();
			continue;
		}
		if (ReadFile(connectedPort, Buffer, SIZE_SERIAL_BUFFER, &BytesIterated, NULL)) {
			SetWindowTextA(TextBox1, Buffer);
		}
	}
	return 0;
}

void SerialWrite(char* buffer, int length)
{
	if (!isConnected) {
		return;
	}
	DWORD BytesIterated;
	WriteFile(connectedPort, buffer, length, &BytesIterated, NULL);
}

void SerialUpdate()
{
	while (RemoveMenu(ComPortListMenu, 0, MF_BYPOSITION));
	int redioLast = 0, radioCurrent = -1;

	for (int i = 1; i < COM_PORT_AMOINT; i++)
	{
		HANDLE port = CreateFileA(
			("\\\\.\\COM" + std::to_string(i)).c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (port != INVALID_HANDLE_VALUE) {
			AppendMenuA(ComPortListMenu, MF_STRING, COM_SELECT_INDEX + i, ("COM" + std::to_string(i)).c_str());
			if (i == selectedPort) {
				radioCurrent = redioLast;
			}
			++redioLast;
		}
		CloseHandle(port);
	}
	if (redioLast) {
		--redioLast;
	}
	if (radioCurrent != -1) {
		CheckMenuItem(ComPortListMenu, radioCurrent, MF_BYPOSITION | MF_CHECKED);
	}
}


