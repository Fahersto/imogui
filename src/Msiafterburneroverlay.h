#include <functional>

namespace imogui
{
	class Renderer;
	class Msiafterburneroverlay
	{

	public:
		void Hook(std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}