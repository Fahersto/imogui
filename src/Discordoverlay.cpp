#include "Discordoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour discordoverlayHook;

	void Discordoverlay::Hook(renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;

#ifdef _WIN64
		switch (api)
		{
		case renderapi::OPENGL:
			assert(false);
			break;
		case renderapi::DIRECTX9:
			assert(false);
			break;
		case renderapi::DIRECTX11:
			int8_t* hookAddress = Utility::Scan("DiscordHook64.dll", "56 57 53 48 83 EC 30 44 89 C6 ");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)discordoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#elif _WIN32
		int8_t* hookAddress = nullptr;
		switch (api)
		{
		case renderapi::OPENGL:
			assert(false);
			break;
		case renderapi::DIRECTX9:
			hookAddress = Utility::Scan("DiscordHook.dll", "55 89 E5 53 57 56 83 EC 1C A1 ? ? ? ? 8B 7D 08 8B 75 14 ");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)discordoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case renderapi::DIRECTX11:
			assert(false);
			break;
		}
#endif

	}

	void Discordoverlay::Unhook()
	{
		discordoverlayHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}