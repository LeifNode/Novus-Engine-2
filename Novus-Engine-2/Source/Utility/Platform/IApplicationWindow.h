/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <string>
#include <functional>
#include "Math/Vector2.h"

namespace novus
{

class IApplicationWindow
{
public:
	virtual ~IApplicationWindow() {}

	virtual void Update() = 0;

	/**
	 *	Gets the width of the window's drawing surface.
	 */
	virtual unsigned int GetWindowWidth() const = 0;

	/**
	 *	Gets the height of the window's drawing surface.
	 */
	virtual unsigned int GetWindowHeight() const = 0;
	
	/**
	 *	Sets the width of the window's drawing surface.
	 */
	virtual void SetWindowWidth(unsigned int width) = 0;

	/**
	 *	Sets the height of the window's drawing surface.
	 */
	virtual void SetWindowHeight(unsigned int height) = 0;

	/**
	 *	Sets the size of the window's drawing surface
	 */
	virtual void SetWindowSize(const Vector2u& Size) = 0;

	virtual std::wstring GetWindowTitle() const = 0;
	virtual void SetWindowTitle(const std::wstring& title) = 0;

	/**
	 *	Gets the position of the top left of the window.
	 *	(This may not necessarily be the same as the top left of the drawing surface depending on if the application has a border)
	 */
	virtual Vector2i GetWindowPosition() const = 0;
	virtual void SetWindowPosition(const Vector2i& position) = 0;

	virtual bool IsFullscreen() const = 0;
	virtual bool SetFullscreen(bool fullscreen) = 0;

	///**
	// *	Add a quit callback. The quit callback gets called when the user closes the window.
	// *	@param The callback function must point to a function that returns void and does not take any arguments
	// */
	//virtual void AddOnQuitCallback(const std::function<void()>& callback) = 0;

	///**
	// *	Add a quit callback. The quit callback gets called when the user closes the window.
	// *	@param The callback function must point to a function that returns void and does not take any arguments
	// */
	//virtual void RemoveOnQuitCallback(const std::function<void()>& callback) = 0;

	///**
	// *	Add a resize callback. The resize callback gets called when the user resizes the window.
	// *	@param The callback function must point to a function that takes a Vector2i as a parameter.
	// *			The Vector2i will receive the new width and height of the drawing surface.
	// */
	//virtual void AddOnResizeCallback(const std::function<void(const Vector2i&)>& callback) = 0;
	//virtual void RemoveOnResizeCallback(const std::function<void(const Vector2i&)>& callback) = 0;

	///**
	// *	Add an activate callback. The activate callback gets called when the window gains or loses focus.
	// *	@param callback The callback delegate must point to a function that returns void and takes a bool as an argument.
	// *						The bool that the callback function receives is whether the application has gained or lost focus.
	// *						true: gained focus, false: lost focus
	// */
	//virtual void AddOnActivateCallback(const std::function<void(bool)>& callback) = 0;

	///**
	// *	Remove an activate callback. The activate callback gets called when the window gains or loses focus.
	// *	@param callback A callback identical to the one that should be removed.
	// */
	//virtual void RemoveOnActivateCallback(const std::function<void(bool)>& callback) = 0;

	///**
	// *	Add a moving callback. The moving callback gets called when the window begins or ends being moved/resized by the user.
	// *	@param callback The callback delegate must point to a function that returns void and takes a bool as an argument.
	// *						The bool that the callback function receives is whether the move event has started or ended
	// *						true: started moving, false: ended moving
	// */
	//virtual void AddOnMoveCallback(const std::function<void(bool)>& callback) = 0;

	virtual void SetOnQuitCallback(const std::function<void()>& callback) = 0;
	virtual void SetOnResizeCallback(const std::function<void(const Vector2i&)>& callback) = 0;
	virtual void SetOnActivateCallback(const std::function<void(bool)>& callback) = 0;
	virtual void SetOnMoveCallback(const std::function<void(bool)>& callback) = 0;
};

};