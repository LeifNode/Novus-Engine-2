#include "AppWnd.h"
#include "AppTest.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	novus::AppWnd* application = new novus::AppWnd(hInstance);

	application->Init();

	application->Run();

	delete application;

	return 0;
}

namespace novus
{

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return _gApplication->MsgProc(hwnd, msg, wParam, lParam);
}

AppWnd::AppWnd(HINSTANCE instance)
	:ClientWidth(1280),
	ClientHeight(720),
	Application(nullptr),
	AppInstance(instance)
{
	_gApplication = this;
}

AppWnd::~AppWnd()
{
	delete Application;
	Application = nullptr;
}

void AppWnd::Init()
{
	InitConsole();
	InitWindow();

	Application = new AppTest();
	Application->Init(WindowHandle);
}

void AppWnd::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Application->Update();
			Application->Render();
		}
	}
}

LRESULT AppWnd::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void AppWnd::InitWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = AppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3D12WndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return;
	}

	DWORD wsStyle = WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, wsStyle, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	WindowHandle = CreateWindow(L"D3D12WndClassName", L"D3D12 Test App", wsStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, AppInstance, NULL);
	if (!WindowHandle)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return;
	}

	ShowWindow(WindowHandle, SW_SHOW);
	UpdateWindow(WindowHandle);

	RECT rect;

	GetWindowRect(WindowHandle, &rect);

	MoveWindow(ConsoleHandle, rect.right, rect.top, 450, rect.bottom - rect.top, true);
}

void AppWnd::InitConsole()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	ConsoleHandle = GetConsoleWindow();

	if (!ConsoleHandle)
	{
		MessageBox(0, L"Console Initialize Failed", 0, 0);
	}
}

}