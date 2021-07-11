#include "Discordoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Discordoverlay::Hook(std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;

		//x64 d3d11
		int8_t* hookAddress = Utility::Scan("DiscordHook64.dll", "56 57 53 48 83 EC 30 44 89 C6 ");
		DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
	}

	void Discordoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}