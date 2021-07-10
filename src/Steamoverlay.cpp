#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"

#include "Renderer.h"

#include "Detour.h"
#include "MidfunctionHook.h"

#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;
	hookftw::MidfunctionHook swapchainPresentMidfunctionHook;

	void Steamoverlay::Hook(std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::d3d11DrawCallback  = drawCallback;
		//int8_t* hookAddress = Utility::ScanBytePattern("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
		
		int8_t* hookAddress = (int8_t*)GetModuleHandleA("GameOverlayRenderer64.dll") + 0x890E0; // , "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");

		DrawHooks::oReturn = (OriginalFn)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedSwapchain());
	}

	void Steamoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}