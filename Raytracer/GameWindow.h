#pragma once
#include "Window.h"
#include "IGameWindowHandler.h"
#include "HDCBufferSwapChain.h"

#include <utility>
#include <memory>

class GameWindow : public IWindowMessageHandler
{
public:
	GameWindow(IGameWindowHandler& handler, const std::wstring& title = L"");

	std::variant<Window::Nothing, Window::Quit> ProcessMessages(size_t maxToProcess = 10);

private:
	Arg::MsgProcessed ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

	static Helpers::Point2D<float> GetPointFromLPARAM(LPARAM lparam);

	std::reference_wrapper<IGameWindowHandler> handler;
	HDCBufferSwapChain swapChain;
	Window window;
};
