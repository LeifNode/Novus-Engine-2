/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <Windows.h>
#include <functional>
#include "IApplicationWindow.h"
#include "Utility/Delegates/MulticastDelegate.h"

namespace novus
{

class WindowsApplicationWindow : public IApplicationWindow
{
public:
	WindowsApplicationWindow(HINSTANCE hInstance);
	virtual ~WindowsApplicationWindow();

	/**
	 *	Creates a window with the specified width and height
	 */
	bool Init(unsigned int width, unsigned int height);

	void Update() override;

	unsigned int GetWindowWidth() const override;
	unsigned int GetWindowHeight() const override;

	void SetWindowWidth(unsigned int width) override;
	void SetWindowHeight(unsigned int height) override;

	void SetWindowSize(const Vector2u& Size) override;

	std::wstring GetWindowTitle() const override;
	void SetWindowTitle(const std::wstring& title) override;

	Vector2i GetWindowPosition() const override;
	void SetWindowPosition(const Vector2i& position) override;

	/**
	*	Gets if the application is in fullscreen mode. Does not do anything on WINAPI windows at the moment
	*/
	bool IsFullscreen() const override;
	bool SetFullscreen(bool fullscreen) override;

	HINSTANCE GetAppInstance() const;
	HWND GetHwnd() const;

	LRESULT _handleWinAPIMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/*void AddOnQuitCallback(const std::function<void()>& callback) override;
	void RemoveOnQuitCallback(const std::function<void()>& callback) override;

	void AddOnResizeCallback(const std::function<void(const Vector2i&)>& callback) override;
	void RemoveOnResizeCallback(const std::function<void(const Vector2i&)>& callback) override;

	void AddOnActivateCallback(const std::function<void(bool)>& callback) override;
	void RemoveOnActivateCallback(const std::function<void(bool)>& callback) override;

	void AddOnMoveCallback(const std::function<void(bool)>& callback) override;
	void RemoveOnMoveCallback(const std::function<void(bool)>& callback) override;*/

	void SetOnQuitCallback(const std::function<void()>& callback) override;
	void SetOnResizeCallback(const std::function<void(const Vector2i&)>& callback) override;
	void SetOnActivateCallback(const std::function<void(bool)>& callback) override;
	void SetOnMoveCallback(const std::function<void(bool)>& callback) override;

	void SetProcessInputCallback(const std::function<bool(HWND, UINT, WPARAM, LPARAM)>& callback);

private:
	void UpdateWindowSize();

private:
	HINSTANCE AppInstance;
	HWND AppWindowHandle;

	DWORD WindowStyle;

	unsigned WindowWidth;
	unsigned WindowHeight;
	Vector2i WindowPosition;

	std::wstring WindowTitle;

	std::function<void(const Vector2i&)>       OnWindowResizeCallback;
	std::function<void()>                      OnQuitCallback;
	std::function<void(bool)>                  OnActivateCallback;
	std::function<void(bool)>                  OnMoveCallback;
	std::function<bool(HWND, UINT, WPARAM, LPARAM)> ProcessInputCallback;
};

namespace detail
{
	static WindowsApplicationWindow* StaticWindowPtr;
}

}