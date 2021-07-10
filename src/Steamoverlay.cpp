#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Detour.h"

namespace imogui
{
	void Steamoverlay::Hook(std::function<void()> drawCallback)
	{
		DrawHooks::d3d11DrawCallback  = drawCallback;
		int8_t* hookAddress = Utility::ScanBytePattern("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
		
		hookftw::Detour swapchainPresentHook;
		
		DrawHooks::oReturn = (OriginalFn)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedSwapchain());
	}

}