#include "DrawHooks.h"




#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace imogui
{
	namespace
	{
		ID3D11Device* pDevice = nullptr;
		ID3D11RenderTargetView* RenderTargetView = NULL;
		ID3D11DeviceContext* pContext = nullptr;

		//viewport
		UINT vps = 1;
		D3D11_VIEWPORT viewport;
		HRESULT hr;

		HWND window;
		WNDPROC oWndProc;


		LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			{
				return true;
			}
			return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
		}
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
			window = sd.OutputWindow;

			//wndprochandler
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui_ImplWin32_Init(window);

			ImGui_ImplDX11_Init(pDevice, pContext);
			ImGui::GetIO().ImeWindowHandle = window;
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

		DrawHooks::d3d11DrawCallback();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return DrawHooks::oReturn(pSwapChain, SyncInterval, Flags);
	}
	int8_t* DrawHooks::GetPointerToHookedSwapchain()
	{
		return (int8_t*)hookD3D11Present;
	}
}