#include <functional>

#include "Renderer.h"

namespace imogui
{
	class Overwolfoverlay
	{

	public:
		void Hook(Renderapi api, std::function<void(Renderer&)> drawCallback);
		void Unhook();
	};
}