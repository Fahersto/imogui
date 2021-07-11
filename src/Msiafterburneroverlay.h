#include <functional>

#include "Renderer.h"

namespace imogui
{
	class Msiafterburneroverlay
	{

	public:
		void Hook(renderapi api, std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}