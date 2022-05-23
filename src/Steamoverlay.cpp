#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

#include "MidfunctionHook.h"

namespace imogui
{
	hookftw::Detour steamoverlayHook;
	hookftw::MidfunctionHook steamoverlaMidfunctionHook;
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

	void Steamoverlay::Hook(Renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback  = drawCallback;
		int8_t* hookAddress = nullptr;

#ifdef _WIN64
		switch (api)
		{
		case Renderapi::OPENGL:
			hookAddress = Utility::Scan("GameOverlayRenderer64.dll", "40 53 48 83 EC 30 48 8B D9 48 8D 54 24");
			steamoverlaMidfunctionHook.Hook(
				hookAddress,
				[](hookftw::context* ctx)
				{
					OpenGLSwapbuffersMidfunction((HDC)ctx->rcx);
				}
			);
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
			DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)steamoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			steamoverlaMidfunctionHook.Hook(
				Utility::Scan("GameOverlayRenderer.dll", "55 8B EC 83 EC 10 8D 45 F0 50"), 
				[](hookftw::context* ctx)
				{
					OpenGLSwapbuffersMidfunction((HDC)*(int32_t*)(ctx->esp+4));
				});
			usedMidfunctionHook = true;
			break;
		case Renderapi::DIRECTX9:
			hookAddress = Utility::Scan("GameOverlayRenderer.dll", "55 8B EC 83 EC 4C 53");
			DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)steamoverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			break;
		}
#endif

	
	}

	void Steamoverlay::Unhook()
	{
		if (usedMidfunctionHook)
		{
			steamoverlaMidfunctionHook.Unhook();
		}
		else
		{
			steamoverlayHook.Unhook();
		}
		InputHandler::UnhookWndProc();
	}

}