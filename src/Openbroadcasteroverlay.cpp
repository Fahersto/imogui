#include "Openbroadcasteroverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour obsOverlayHook;

	void Openbroadcasteroverlay::Hook(Renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		int8_t* hookAddress = nullptr;
#ifdef _WIN64
		switch (api)
		{
		case Renderapi::OPENGL:
			assert(false);
			break;
		case Renderapi::DIRECTX9:
			hookAddress = Utility::Scan("graphics-hook64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 40 80 3D ? ? ? ? ? 49 8B D9 49 8B F8 48 C7 44 24 ? ? ? ? ? 48 8B EA 48 8B F1 75 05 E8 ? ? ? ? 48 8D 54 24 ? 48 8B CE E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 44 24");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("graphics-hook64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 20 48 8B 05 ? ? ? ?");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			assert(false);
			break;
		case Renderapi::DIRECTX9:
			hookAddress = Utility::Scan("graphics-hook32.dll", "55 8B EC 51 80 3D ? ? ? ? ? 53 56 8B 75 08");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("graphics-hook32.dll", "55 8B EC A1 ? ? ? ? 0F 57 C0");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#endif
	}

	void Openbroadcasteroverlay::Unhook()
	{
		obsOverlayHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}