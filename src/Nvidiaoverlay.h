#include <functional>

#include "Renderer.h"

namespace imogui
{
	class Nvidiaoverlay
	{

	public:
		/**
		* Inject into "NVIDIA Share.exe".
		*/
		void Hook(Renderapi api, std::function<void(Renderer&)> drawCallback);
		void Unhook();
	};
}