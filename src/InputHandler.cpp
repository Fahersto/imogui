#include "InputHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace imogui
{
	namespace
	{
		WNDPROC oWndProc_;
		HWND window_;
	}

	LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		{
			return true;
		}
		return CallWindowProc(oWndProc_, hWnd, uMsg, wParam, lParam);
	}

	void InputHandler::HookWndProc(HWND window)
	{
		window_ = window;
		oWndProc_ = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
	}

	void InputHandler::UnhookWndProc()
	{
		(WNDPROC)SetWindowLongPtr(window_, GWLP_WNDPROC, (LONG_PTR)oWndProc_);
	}
}