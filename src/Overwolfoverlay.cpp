#include "Overwolfoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Overwolfoverlay::Hook(renderapi api, std::function<void(Renderer*)> drawCallback)
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
			assert(false);
			//not working
			int8_t* hookAddress = Utility::Scan("dxgi.dll", "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60");
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

	void Overwolfoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}