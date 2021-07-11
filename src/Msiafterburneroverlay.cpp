#include "Msiafterburneroverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Msiafterburneroverlay::Hook(renderapi api, std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
	
#ifdef _WIN64
		switch (api)
		{
		case renderapi::opengl:
			assert(false);
			break;
		case renderapi::directx9:
			assert(false);
			break;
		case renderapi::directx11:
			int8_t* hookAddress = Utility::Scan("RTSSHooks64.dll", "48 89 5C 24 ? 55 56 57 48 83 EC 30 48 8B");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
		}
#elif _WIN32
		switch (api)
		{
		case renderapi::opengl:
			assert(false);
			break;
		case renderapi::directx9:
			assert(false);
			break;
		case renderapi::directx11:
			assert(false);
			break;
		}
#endif
	}

	void Msiafterburneroverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}