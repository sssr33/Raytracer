#pragma once

#include <Windows.h>
#include <Helpers/Strong.h>

namespace Arg
{
	using MsgProcessed = Helpers::Strong<bool, struct MsgProcessedParameter>;
}

class IWindowMessageHandler
{
public:
	virtual ~IWindowMessageHandler() = default;

	virtual Arg::MsgProcessed ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};
