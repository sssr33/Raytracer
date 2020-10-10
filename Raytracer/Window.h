#pragma once

#include <string>
#include <memory>
#include <type_traits>
#include <variant>
#include <Windows.h>
#include <Helpers/Strong.h>

namespace Arg
{
	using MsgProcessed = Helpers::Strong<bool, struct MsgProcessedParameter>;
}

class Window
{
public:
	struct Nothing {};
	struct Quit {};

	Window(const std::wstring& title = L"");
	Window(const Window&) = delete;
	Window(Window&& other) noexcept;

	Window& operator=(Window other) noexcept;

	friend void swap(Window& a, Window& b) noexcept;

	std::variant<Nothing, Quit> ProcessMessages(size_t maxToProcess = 10);

private:
	class WndClassRegistration
	{
	public:
		WndClassRegistration();
		~WndClassRegistration();

		void Unregister();
	};

	struct HwndDeleter
	{
		void operator()(HWND hwnd);
	};

	virtual Arg::MsgProcessed ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static HINSTANCE GetHInstance();
	static std::shared_ptr<WndClassRegistration> GetWndRegistration();

	static const std::wstring WndClassName;

	std::shared_ptr<WndClassRegistration> wndReg;
	std::unique_ptr<std::remove_pointer_t<HWND>, HwndDeleter> hwnd;
	bool quit = false;
};