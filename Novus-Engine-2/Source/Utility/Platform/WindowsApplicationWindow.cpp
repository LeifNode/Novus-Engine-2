#include "WindowsApplicationWindow.h"

namespace novus
{

/**
 *	Invokes a given std::function if it points to a valid function
 */
template <typename return_t, typename ... params>
void InvokeIfValid(std::function<return_t(params...)>& func, params... args)
{
	if (func)
		func(args...);
}

LRESULT CALLBACK _mainWndProcWinAPI(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return detail::StaticWindowPtr->_handleWinAPIMsg(hwnd, msg, wParam, lParam);
}

WindowsApplicationWindow::WindowsApplicationWindow(HINSTANCE hInstance)
	:AppInstance(hInstance)
{
	detail::StaticWindowPtr = this;
}

WindowsApplicationWindow::~WindowsApplicationWindow()
{}

bool WindowsApplicationWindow::Init(unsigned int width, unsigned int height)
{
	//Might want to change this to be specifiable if I want to have multple windows at once
	static const wchar_t* windowClassName = L"D3D12AppWndClass"; 

	WindowWidth = width;
	WindowHeight = height;

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = _mainWndProcWinAPI;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = AppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = windowClassName;

	if (!RegisterClassW(&wc))
	{
		return false;
	}

	WindowStyle = WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	//Compute window dimensions taking the border of the window into account
	RECT rect = { 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) };
	AdjustWindowRect(&rect, WindowStyle, false);
	int actualWidth = rect.right - rect.left;
	int actualHeight = rect.bottom - rect.top;

	AppWindowHandle = CreateWindow(windowClassName, WindowTitle.c_str(), WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, actualWidth, actualHeight, nullptr, nullptr, AppInstance, nullptr);

	if (!AppWindowHandle)
	{
		return false;
	}

	ShowWindow(AppWindowHandle, SW_SHOW);
	UpdateWindow(AppWindowHandle);

	GetWindowRect(AppWindowHandle, &rect);

	WindowPosition.x = rect.left;
	WindowPosition.y = rect.top;

	return true;
}

void WindowsApplicationWindow::Update()
{
	MSG msg = { 0 };

	//Process all messages received in the last frame from the window and forward them as necessary
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, AppWindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			break;
		}
	}

	if (msg.message == WM_QUIT)
	{
		InvokeIfValid(OnQuitCallback);
	}
}

unsigned int WindowsApplicationWindow::GetWindowWidth() const
{
	return WindowWidth;
}

unsigned int WindowsApplicationWindow::GetWindowHeight() const
{
	return WindowHeight;
}

void WindowsApplicationWindow::SetWindowWidth(unsigned int width)
{
	WindowWidth = width;

	UpdateWindowSize();
}

void WindowsApplicationWindow::SetWindowHeight(unsigned int height)
{
	WindowHeight = height;

	UpdateWindowSize();
}

void WindowsApplicationWindow::SetWindowSize(const Vector2u & Size)
{
	WindowWidth = Size.x;
	WindowHeight = Size.y;

	UpdateWindowSize();
}

std::wstring WindowsApplicationWindow::GetWindowTitle() const
{
	return WindowTitle;
}

void WindowsApplicationWindow::SetWindowTitle(const std::wstring & title)
{
	WindowTitle = title;

	assert(SetWindowTextW(AppWindowHandle, title.c_str()));
}

Vector2i WindowsApplicationWindow::GetWindowPosition() const
{
	return WindowPosition;
}

void WindowsApplicationWindow::SetWindowPosition(const Vector2i & position)
{
	WindowPosition = position;

	//Compute window dimensions taking the border of the window into account
	RECT rect = { 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) };
	AdjustWindowRect(&rect, WindowStyle, false);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	assert(SetWindowPos(AppWindowHandle, HWND_TOP, WindowPosition.x, WindowPosition.y, width, height, SWP_SHOWWINDOW | SWP_NOOWNERZORDER));
}

bool WindowsApplicationWindow::IsFullscreen() const
{ 
	//Maybe this could be used for windowed fullscreen mode on windows
	return false;
}

bool WindowsApplicationWindow::SetFullscreen(bool fullscreen)
{
	return false;
}

HINSTANCE WindowsApplicationWindow::GetAppInstance() const
{
	return AppInstance;
}

HWND WindowsApplicationWindow::GetHwnd() const
{
	return AppWindowHandle;
}

LRESULT WindowsApplicationWindow::_handleWinAPIMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle input from mouse/keyboard along with window resize/move events and close events, then forward to event callbacks
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			InvokeIfValid(OnActivateCallback, false);
		else
			InvokeIfValid(OnActivateCallback, true);
		return 0;
	case WM_SIZE:
		WindowWidth = static_cast<unsigned int>(LOWORD(lParam));
		WindowHeight = static_cast<unsigned int>(HIWORD(lParam));

		InvokeIfValid<void, const Vector2i&>(OnWindowResizeCallback, Vector2i(WindowWidth, WindowHeight));
		return 0;
	case WM_ENTERSIZEMOVE:
		InvokeIfValid(OnMoveCallback, true);
		return 0;
	case WM_EXITSIZEMOVE:
		InvokeIfValid(OnMoveCallback, false);
		return 0;
	case WM_DESTROY:
		//Forward quit message on to message pump to go and call the OnQuitCallback
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		//Don't beep when alt-enter is pressed
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		//Lock the minimum window size to 200x200
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	default:
		if (ProcessInputCallback)
			//Callback to the input system to handle any windows messages for input
			if (ProcessInputCallback(hwnd, msg, wParam, lParam)) 
				return 0;
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//void WindowsApplicationWindow::AddOnQuitCallback(const std::function<void()>& callback)
//{
//	OnQuitCallback += callback;
//}
//
//void WindowsApplicationWindow::RemoveOnQuitCallback(const std::function<void()>& callback)
//{
//	OnQuitCallback -= callback;
//}
//
//void WindowsApplicationWindow::AddOnResizeCallback(const std::function<void(const Vector2i&)>& callback)
//{
//	OnWindowResizeCallback += callback;
//}
//
//void WindowsApplicationWindow::RemoveOnResizeCallback(const std::function<void(const Vector2i&)>& callback)
//{
//	OnWindowResizeCallback -= callback;
//}
//
//void WindowsApplicationWindow::AddOnActivateCallback(const std::function<void(bool)>& callback)
//{
//	OnActivateCallback += callback;
//}
//
//void WindowsApplicationWindow::RemoveOnActivateCallback(const std::function<void(bool)>& callback)
//{
//	OnActivateCallback -= callback;
//}
//
//void WindowsApplicationWindow::AddOnMoveCallback(const std::function<void(bool)>& callback)
//{
//	OnMoveCallback += callback;
//}
//
//void WindowsApplicationWindow::RemoveOnMoveCallback(const std::function<void(bool)>& callback)
//{
//	OnMoveCallback -= callback;
//}

void WindowsApplicationWindow::SetOnQuitCallback(const std::function<void()>& callback)
{
	OnQuitCallback = callback;
}

void WindowsApplicationWindow::SetOnResizeCallback(const std::function<void(const Vector2i&)>& callback)
{
	OnWindowResizeCallback = callback;
}

void WindowsApplicationWindow::SetOnActivateCallback(const std::function<void(bool)>& callback)
{
	OnActivateCallback = callback;
}

void WindowsApplicationWindow::SetOnMoveCallback(const std::function<void(bool)>& callback)
{
	OnMoveCallback = callback;
}

void WindowsApplicationWindow::SetProcessInputCallback(const std::function<bool(HWND, UINT, WPARAM, LPARAM)>& callback)
{
	ProcessInputCallback = callback;
}

void WindowsApplicationWindow::UpdateWindowSize()
{
	//Compute window dimensions taking the border of the window into account
	RECT rect = { 0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight) };
	AdjustWindowRect(&rect, WindowStyle, false);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	assert(SetWindowPos(AppWindowHandle, HWND_TOP, WindowPosition.x, WindowPosition.y, width, height, SWP_SHOWWINDOW | SWP_NOOWNERZORDER));

	InvokeIfValid<void, const Vector2i&>(OnWindowResizeCallback, Vector2i(WindowWidth, WindowHeight));
}

}