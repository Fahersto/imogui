#include <functional>

#include "Renderer.h"

namespace imogui
{
	class Openbroadcasteroverlay
	{

	public:
		/**
		* Inject into "NVIDIA Share.exe"
		*/
		void Hook(renderapi api, std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}