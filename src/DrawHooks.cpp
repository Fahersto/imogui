#include "DrawHooks.h"

#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_dx11.h"

#include "InputHandler.h"


namespace imogui
{
	std::function<void(Renderer*)> DrawHooks::renderCallback = nullptr;

	// original opengl swapbuffers function
	OpenGL_SwapBuffers DrawHooks::originalOpenGLSwapBuffers = nullptr;

	Direct3DDevice9_Present DrawHooks::originalDirect3DDevice9Present = nullptr;

	DirectX11_IDXGISwapChain_Present DrawHooks::oDirectX11SwapchainPresent = nullptr;
	

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

	int64_t HkHwglSwapBuffers(HDC hdc)
	{
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

		}

		DrawHooks::renderCallback(Renderer::Get());


		return DrawHooks::originalOpenGLSwapBuffers(hdc);
	}

	int64_t __stdcall hkD3D9Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND wnd_override, const RGNDATA* dirty_region)
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

			InputHandler::HookWndProc(creationParameters.hFocusWindow);

			ImGui_ImplWin32_Init(creationParameters.hFocusWindow);
			ImGui_ImplDX9_Init(device);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Renderer::Get()->BeginScene();

		DrawHooks::renderCallback(Renderer::Get());

		Renderer::Get()->EndScene();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return DrawHooks::originalDirect3DDevice9Present(device, src, dest, wnd_override, dirty_region);
	}

	HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
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
				return hr;
			}

			hr = pDevice->CreateRenderTargetView(backbuffer, nullptr, &RenderTargetView);
			backbuffer->Release();
			if (FAILED(hr))
			{
				return hr;
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

		return DrawHooks::oDirectX11SwapchainPresent(pSwapChain, SyncInterval, Flags);
	}


	int8_t* DrawHooks::GetPointerToHookedSwapBuffers()
	{
		assert(false);
		return nullptr;
	}

	int8_t* DrawHooks::GetPointerToHookedDirect3DDevice9Present()
	{
		return (int8_t*)hkD3D9Present;
	}

	int8_t* DrawHooks::GetPointerToHookedDirectX11SwapchainPresent()
	{
		return (int8_t*)hookD3D11Present;
	}
}