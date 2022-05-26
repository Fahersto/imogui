#include <functional>
#include <d3d9.h>
#include <d3d11.h>

#include "MidfunctionHook.h"

namespace imogui
{
	using Direct3DDevice9_Present = int(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	using DirectX11_IDXGISwapChain_Present = int64_t(*)(IDXGISwapChain*, int64_t a2, int64_t a3);
	using DirectX11_MidfunctionHook = void(__fastcall*) (hookftw::context* ctx);

	class Renderer;
	class DrawHooks
	{

	public:
		static std::function<void(Renderer&)> renderCallback;
		
		// opengl
		static void OpenGLSwapbuffersMidfunction(HDC hDc);

		// directX9
		static Direct3DDevice9_Present originalDirect3DDevice9Present;
		static int8_t* GetPointerToHookedDirect3DDevice9Present();

		// directX11
		static DirectX11_IDXGISwapChain_Present oDirectX11SwapchainPresent;
		static int8_t* GetPointerToHookedDirectX11SwapchainPresent();
		
	};
}