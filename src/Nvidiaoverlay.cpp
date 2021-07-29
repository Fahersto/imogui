#include "Nvidiaoverlay.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"

namespace imogui
{
	hookftw::Detour swapchainPresentHook;

	void Nvidiaoverlay::Hook(renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		//x64 d3d11 
		//int8_t* hookAddress = Utility::Scan(NULL, "48 89 5C 24 ? 55 56 57 48 83 EC 30 48 8B");


		ID3D11Device* g_pd3dDevice = nullptr;
		IDXGISwapChain* g_pSwapChain = nullptr;

		D3D_FEATURE_LEVEL featLevel;
		DXGI_SWAP_CHAIN_DESC sd{ 0 };
		sd.BufferCount = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.Height = 800;
		sd.BufferDesc.Width = 600;
		sd.BufferDesc.RefreshRate = { 60, 1 };
		sd.OutputWindow = GetForegroundWindow();
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featLevel, nullptr);

		if (FAILED(hr))
		{
			MessageBox(GetForegroundWindow(), "Failed to create device and swapchain.", "Fatal Error", MB_ICONERROR);
			return;
		}
		DWORD_PTR* pSwapChainVTable = nullptr;
		pSwapChainVTable = (DWORD_PTR*)(g_pSwapChain);
		pSwapChainVTable = (DWORD_PTR*)(pSwapChainVTable[0]);

		DrawHooks::oDirectX11SwapchainPresent = (DirectX11_IDXGISwapChain_Present)swapchainPresentHook.Hook((int8_t*)pSwapChainVTable[8], DrawHooks::GetPointerToHookedDirectX11SwapchainPresent());
	}

	void Nvidiaoverlay::Unhook()
	{
		swapchainPresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}