#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "MidfunctionHook.h"

namespace imogui
{
	hookftw::Detour steamoverlayHook;
	hookftw::MidfunctionHook steamoverlaMidfunctionHook;

	void Steamoverlay::Hook(renderapi api, std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::renderCallback  = drawCallback;

		int8_t* hookAddress = nullptr;

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
			hookAddress = Utility::Scan("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)steamoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#elif _WIN32
		switch (api)
		{
		case renderapi::opengl:
			assert(false);
			break;
		case renderapi::directx9:
			hookAddress = Utility::Scan("GameOverlayRenderer.dll", "55 8B EC 83 EC 4C 53");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)steamoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case renderapi::directx11:
			assert(false);
			break;
		}
#endif

	
	}

	void Steamoverlay::Unhook()
	{
		steamoverlayHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}