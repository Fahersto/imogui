#include <functional>
#include <d3d11.h>

namespace imogui
{
	using OriginalFn = int64_t(*)(IDXGISwapChain*, int64_t a2, int64_t a3);

	class DrawHooks
	{

	public:
		static OriginalFn oReturn;
		static std::function<void()> d3d11DrawCallback;
		static int8_t* GetPointerToHookedSwapchain();
	};
}