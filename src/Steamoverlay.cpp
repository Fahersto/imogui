#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Steamoverlay::Hook(std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::d3d11DrawCallback  = drawCallback;
		//x64 d3d11 
		int8_t* hookAddress = Utility::Scan("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
		DrawHooks::oReturn = (OriginalFn)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedSwapchain());
	}

	void Steamoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}