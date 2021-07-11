#include "Steamoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "MidfunctionHook.h"
#include "InputHandler.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;


#ifdef _WIN64

#elif _WIN32
	ID3D11Device* pDevice = nullptr;
	ID3D11RenderTargetView* RenderTargetView = NULL;
	ID3D11DeviceContext* pContext = nullptr;

	//viewport
	UINT vps = 1;
	D3D11_VIEWPORT viewport;
	HRESULT hr;
	void __fastcall hkd3d11Swapchain(hookftw::context* ctx)
	{
		IDXGISwapChain* pSwapChain = (IDXGISwapChain*)(ctx->ecx);

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

		Renderer::Get()->BeginScene();

		DrawHooks::renderCallback(Renderer::Get());

		Renderer::Get()->EndScene();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

#endif

	void Steamoverlay::Hook(std::function<void(Renderer*)> drawCallback)
	{
		DrawHooks::renderCallback  = drawCallback;

		int8_t* hookAddress = nullptr;

#ifdef _WIN64
		//x64 d3d11 
		hookAddress = Utility::Scan("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41");
		DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)swapchainPresentHook.Hook(hookAddress, DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
#elif _WIN32
		assert(false);
#endif

	
	}

	void Steamoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}