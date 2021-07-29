#include "Openbroadcasteroverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Openbroadcasteroverlay::Hook(renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		//x64 d3d11 

		/*
		int8_t* hookAddress = Utility::Scan("graphics-hook64.dll", "48 8D 05 ? ? ? ? 48 89 3D");

		int32_t relativePart = *(int32_t*)(hookAddress + 3);
		int32_t sizeofinstruction = 7;
		hookAddress = hookAddress + relativePart + sizeofinstruction;
		*/

		int8_t* hookAddress = Utility::Scan("graphics-hook64.dll", "?") + 0x8b50;


		DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
	}

	void Openbroadcasteroverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}