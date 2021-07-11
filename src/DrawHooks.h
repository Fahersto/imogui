#include <functional>
#include <d3d9.h>
#include <d3d11.h>

namespace imogui
{
	using OpenGL_SwapBuffers = int64_t(*)(HDC a2);
	using Direct3DDevice9_Present = int(__stdcall*)(IDirect3DDevice9*, __int64 a2, __int64 a3, __int64 a4, __int64 a5);
	using DirectX11_IDXGISwapChain_Present = int64_t(*)(IDXGISwapChain*, int64_t a2, int64_t a3);
	

	class Renderer;
	class DrawHooks
	{

	public:
		static std::function<void(Renderer*)> renderCallback;


		// opengl
		static OpenGL_SwapBuffers originalOpenGLSwapBuffers;
		static int8_t* GetPointerToHookedSwapBuffers();
		
		// directX9
		static Direct3DDevice9_Present originalDirect3DDevice9Present;
		static int8_t* GetPointerToHookedDirect3DDevice9Present();

		// directX11
		static DirectX11_IDXGISwapChain_Present oDirectX11SwapchainPresent;
		static int8_t* GetPointerToHookedDirectX11SwapchainPresent();
	};
}