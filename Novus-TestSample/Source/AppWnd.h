#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace novus
{

class AppWnd
{
public:
	AppWnd(HINSTANCE instance);
	~AppWnd();

	void Init();

	void Run();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void InitWindow();
	void InitConsole();

private:
	HINSTANCE AppInstance;
	HWND WindowHandle;
	HWND ConsoleHandle;

	int ClientWidth;
	int ClientHeight;

	class AppTest* Application;
};

namespace
{
	AppWnd* _gApplication;
}

}