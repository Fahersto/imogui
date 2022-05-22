#include "Openbroadcasteroverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "MidfunctionHook.h"
#include "InputHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_dx11.h"

#include <Windows.h>

namespace imogui
{
	//hookftw::Detour obsOverlayHook;
	hookftw::MidfunctionHook midFunction;

	namespace
	{
		ID3D11Device* pDevice = nullptr;
		ID3D11RenderTargetView* RenderTargetView = NULL;
		ID3D11DeviceContext* pContext = nullptr;

		//viewport
		UINT vps = 1;
		D3D11_VIEWPORT viewport;
		HRESULT hr;
	}

	void  D3D9PresentMidfunction(IDirect3DDevice9* device)
	{
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			D3DDEVICE_CREATION_PARAMETERS creationParameters;

			device->GetCreationParameters(&creationParameters);

			RECT rect;
			GetWindowRect(creationParameters.hFocusWindow, &rect);
			Renderer::Get().SetWidth(rect.right);
			Renderer::Get().SetHeight(rect.bottom);


			InputHandler::HookWndProc(creationParameters.hFocusWindow);

			ImGui_ImplWin32_Init(creationParameters.hFocusWindow);
			ImGui_ImplDX9_Init(device);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Renderer::Get().BeginScene();

		DrawHooks::renderCallback(Renderer::Get());

		Renderer::Get().EndScene();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void D3D11PresentMidfunction(IDXGISwapChain* pSwapChain)
	{
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
				pDevice->GetImmediateContext(&pContext);
			}

			//imgui
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard; //control menu with mouse
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

			RECT rect;
			GetWindowRect(sd.OutputWindow, &rect);
			Renderer::Get().SetWidth(rect.right);
			Renderer::Get().SetHeight(rect.bottom);

			InputHandler::HookWndProc(sd.OutputWindow);

			ImGui_ImplWin32_Init(sd.OutputWindow);

			ImGui_ImplDX11_Init(pDevice, pContext);
			ImGui::GetIO().ImeWindowHandle = sd.OutputWindow;
		}

		if (RenderTargetView == nullptr)
		{
			pContext->RSGetViewports(&vps, &viewport);

			ID3D11Texture2D* backbuffer = nullptr;
			hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
			if (FAILED(hr))
			{
				return;
			}

			hr = pDevice->CreateRenderTargetView(backbuffer, nullptr, &RenderTargetView);
			backbuffer->Release();
			if (FAILED(hr))
			{
				return;
			}
		}
		else
		{
			pContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Renderer::Get().BeginScene();

		DrawHooks::renderCallback(Renderer::Get());

		Renderer::Get().EndScene();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

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
			assert(false);
			//hookAddress = Utility::Scan("graphics-hook64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 40 80 3D ? ? ? ? ? 49 8B D9 49 8B F8 48 C7 44 24 ? ? ? ? ? 48 8B EA 48 8B F1 75 05 E8 ? ? ? ? 48 8D 54 24 ? 48 8B CE E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 44 24");
			//DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			hookAddress = Utility::Scan("graphics-hook64.dll", "44 8B C6 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 1C C8 48 8B CF");
			
			midFunction.Hook(hookAddress, 
				[](hookftw::context* ctx) {
					D3D11PresentMidfunction((IDXGISwapChain*)ctx->rdi);
				}
			);
			break;
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			assert(false);
			break;
		case Renderapi::DIRECTX9:
			hookAddress = Utility::Scan("graphics-hook32.dll", "68 ? ? ? ? E8 ? ? ? ? 83 C4 0C FF 75 18");
			midFunction.Hook(hookAddress,
				[](hookftw::context* ctx) {
					D3D9PresentMidfunction((IDirect3DDevice9*)ctx->esi);
				}
			);
			//DrawHooks::originalDirect3DDevice9Present = (Direct3DDevice9_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirect3DDevice9Present());
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			//hookAddress = Utility::Scan("graphics-hook32.dll", "55 8B EC A1 ? ? ? ? 0F 57 C0");
			//DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)obsOverlayHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
			break;
		}
#endif
	}

	void Openbroadcasteroverlay::Unhook()
	{
		//obsOverlayHook.Unhook();
		midFunction.Unhook();
		InputHandler::UnhookWndProc();
	}

}