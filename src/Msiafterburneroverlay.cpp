#include "Msiafterburneroverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

#include "MidfunctionHook.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

namespace imogui
{
	hookftw::Detour msiSwapchainPresentHook;
	hookftw::MidfunctionHook msiMidfunctionHook;
	bool usedMidfunctionHook = false;

	void Msiafterburneroverlay::Hook(Renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		int8_t* hookAddress = nullptr;
#ifdef _WIN64
		switch (api)
		{
		case Renderapi::OPENGL:
			hookAddress = Utility::Scan("RTSSHooks64.dll", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 F0 0F BA 2D ? ? ? ? ? 48 8B D9 73 2D FF 15 ? ? ? ? F0 0F BA 2D ? ? ? ? ? 8B F8 73 1A FF 15 ? ? ? ? 2B C7 3D ? ? ? ? 77 0B F0 0F BA 2D ? ? ? ? ? 72 E6 F6 05 ? ? ? ? ? 48 89 1D ? ? ? ? 74 16 8B 0D ? ? ? ? 48 8B 35 ? ? ? ? 48 8D 3D ? ? ? ? F3 A4 8B 0D ? ? ? ? 48 8B 3D ? ? ? ? 48 8D 35 ? ? ? ? F3 A4 FF 15 ? ? ? ? 48 8B CB FF 15 ? ? ? ? 89 05 ? ? ? ? FF 15 ? ? ? ? 83 3D ? ? ? ? ? 75 26 F6 05 ? ? ? ? ? 8B 0D ? ? ? ? 48 8B 3D ? ? ? ? 48 8D 35 ? ? ? ? 75 07 48 8D 35 ? ? ? ? F3 A4 8B 05 ? ? ? ? 48 8B 5C 24 ? 48 8B 74 24 ? C7 05 ? ? ? ? ? ? ? ? 48 83 C4 20 5F C3 48 89 5C 24");
			msiMidfunctionHook.Hook(hookAddress,
				[](hookftw::context* ctx) {
					DrawHooks::OpenGLSwapbuffersMidfunction((HDC)ctx->rcx);
				}
			);
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("RTSSHooks64.dll", "48 89 5C 24 ? 55 56 57 48 83 EC 30 48 8B");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)msiSwapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			hookAddress = Utility::Scan("RTSSHooks.dll", "9C 60 F0 0F BA 2D ? ? ? ? ? 73 24 FF 15 ? ? ? ? 05 ? ? ? ? 8B C8 F0 0F BA 2D ? ? ? ? ? 73 0C 51 FF 15 ? ? ? ? 59 3B C1 72 E9 8B EC 83 C5 28 03 2D ? ? ? ? 89 2D ? ? ? ? 8B 45 FC A3 ? ? ? ? B8 ? ? ? ? 89 45 FC BE ? ? ? ? 8B 3D ? ? ? ? B9 ? ? ? ? F3 A4 FF 15 ? ? ? ? 61 9D FF 25 ? ? ? ? 9C 60 A3 ? ? ? ? F7 05 ? ? ? ? ? ? ? ? 75 18 FF 15 ? ? ? ? BE ? ? ? ? 8B 3D ? ? ? ? B9 ? ? ? ? F3 A4 61 9D C7 05 ? ? ? ? ? ? ? ? FF 25 ? ? ? ? CC C7 05 ? ? ? ? ? ? ? ? E9 ? ? ? ? CC E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 9C 60 F0 0F BA 2D ? ? ? ? ? 73 24 FF 15 ? ? ? ? 05 ? ? ? ? 8B C8 F0 0F BA 2D");
			msiMidfunctionHook.Hook(hookAddress,
				[](hookftw::context* ctx) {
					DrawHooks::OpenGLSwapbuffersMidfunction((HDC)*(int32_t*)(ctx->esp+4));
				}
			);
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			break;
		}
#endif
	}

	void Msiafterburneroverlay::Unhook()
	{
		if (usedMidfunctionHook)
		{
			msiMidfunctionHook.Unhook();
		}
		else
		{
			msiSwapchainPresentHook.Unhook();
		}
		InputHandler::UnhookWndProc();
	}

}