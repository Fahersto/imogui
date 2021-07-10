#pragma once
#include <Windows.h>



namespace imogui
{
	class InputHandler
	{
	public:

		static void HookWndProc(HWND window);
		static void UnhookWndProc();
	};
	
}