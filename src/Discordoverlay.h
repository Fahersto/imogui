#include <functional>

#include "Renderer.h"

namespace imogui
{
	class Renderer;
	class Discordoverlay
	{

	public:
		void Hook(renderapi api, std::function<void(Renderer&)> drawCallback);
		void Unhook();
	};
}