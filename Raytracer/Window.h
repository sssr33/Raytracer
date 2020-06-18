#pragma once

#include <string>
#include <memory>
#include <Windows.h>

class Window
{
public:
	Window();

private:
	class WndClassRegistration
	{
	public:
		WndClassRegistration();
		~WndClassRegistration();

		void Unregister();
	};

	static const std::wstring WndClassName;

	std::shared_ptr<WndClassRegistration> wndReg;

	void CreateWnd();

	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static HINSTANCE GetHInstance();
	static std::shared_ptr<WndClassRegistration> GetWndRegistration();
};