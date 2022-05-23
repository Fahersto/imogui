#include "Discordoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Detour.h"
#include "InputHandler.h"
#include "MidfunctionHook.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

namespace imogui
{
	hookftw::Detour discordoverlayHook;
	hookftw::MidfunctionHook discordMidfunctionHook;
	bool usedMidfunctionHook = false;

	void OpenGLSwapbuffersMidfunction(HDC hDc)
	{
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			HWND hWnd = WindowFromDC(hDc);

			RECT rect;
			GetClientRect(hWnd, &rect);
			Renderer::Get().SetWidth(rect.right);
			Renderer::Get().SetHeight(rect.bottom);


			InputHandler::HookWndProc(hWnd);

			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplOpenGL3_Init();
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Renderer::Get().BeginScene();

		DrawHooks::renderCallback(Renderer::Get());

		Renderer::Get().EndScene();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Discordoverlay::Hook(Renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		int8_t * hookAddress = nullptr;

#ifdef _WIN64
		switch (api)
		{
		case Renderapi::OPENGL:	
			hookAddress = Utility::Scan("DiscordHook64.dll", "FF 15 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 84 24 ? ? ? ? 48 89 B4 24 ? ? ? ? 4C");
			discordMidfunctionHook.Hook(hookAddress,
				[](hookftw::context* ctx) {
					OpenGLSwapbuffersMidfunction((HDC)ctx->rcx);
				}
			);
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("DiscordHook64.dll", "56 57 53 48 83 EC 30 44 89 C6");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)discordoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			hookAddress = Utility::Scan("DiscordHook.dll", "FF 15 ? ? ? ? 8D 84 24 ? ? ? ? C7 84 24 ? ? ? ? ? ? ? ? C7 84 24 ? ? ? ? ? ? ? ? 89 9C 24");
			discordMidfunctionHook.Hook(hookAddress,
				[](hookftw::context* ctx) {
					OpenGLSwapbuffersMidfunction((HDC)*(int32_t*)ctx->esp);
				}
			);
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			hookAddress = Utility::Scan("DiscordHook.dll", "55 89 E5 53 57 56 83 EC 1C A1 ? ? ? ? 8B 7D 08 8B 75 14");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)discordoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			break;
		}
#endif

	}

	void Discordoverlay::Unhook()
	{
		if (usedMidfunctionHook)
		{
			discordMidfunctionHook.Unhook();
		}
		else
		{
			discordoverlayHook.Unhook();
		}
		InputHandler::UnhookWndProc();
	}

}