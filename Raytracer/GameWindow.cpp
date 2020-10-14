#include "GameWindow.h"

#include <Windowsx.h>
#include <Helpers/is.h>

GameWindow::GameWindow(IGameWindowHandler& handler, const std::wstring& title)
	: handler(handler)
	, window(this, title)
{}

std::variant<Window::Nothing, Window::Quit> GameWindow::ProcessMessages(size_t maxToProcess)
{
	std::variant<Window::Nothing, Window::Quit> res = this->window.ProcessMessages(maxToProcess);

	if (!is<Window::Quit>(res))
	{
		this->handler.get().GameLoop(this->backBuffer);
	}

	return res;
}

Arg::MsgProcessed GameWindow::ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
		this->handler.get().OnMouseLeftPress(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_LBUTTONUP:
	{
		this->handler.get().OnMouseLeftRelease(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_RBUTTONDOWN:
	{
		this->handler.get().OnMouseRightPress(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_RBUTTONUP:
	{
		this->handler.get().OnMouseRightRelease(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_MBUTTONDOWN:
	{
		this->handler.get().OnMouseMiddlePress(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_MBUTTONUP:
	{
		this->handler.get().OnMouseMiddleRelease(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_MOUSEWHEEL:
	{
		float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam));
		this->handler.get().OnMouseWheel(delta);
		break;
	}
	case WM_MOUSEMOVE:
	{
		this->handler.get().OnMouseMove(GameWindow::GetPointFromLPARAM(lparam));
		break;
	}
	case WM_SIZE:
	{
		Helpers::Size2D<uint32_t> size;

		size.width = static_cast<uint32_t>(LOWORD(lparam));
		size.height = static_cast<uint32_t>(HIWORD(lparam));

		this->backBuffer.SetLastSize(hwnd, size);
		this->handler.get().OnResize(size);
		break;
	}
	case WM_ERASEBKGND:
		// http://www.catch22.net/tuts/win32/flicker-free-drawing#
		// https://stackoverflow.com/questions/35830279/c-win32-gdi-double-buffering
		break;
	case WM_PAINT:
	{
		this->handler.get().OnRepaint(this->backBuffer);
		break;
	}
	default:
		return Arg::MsgProcessed(false);
	}

	return Arg::MsgProcessed(true);
}

Helpers::Point2D<float> GameWindow::GetPointFromLPARAM(LPARAM lparam)
{
	Helpers::Point2D<float> pt;

	pt.x = static_cast<float>(GET_X_LPARAM(lparam));
	pt.y = static_cast<float>(GET_Y_LPARAM(lparam));

	return pt;
}
