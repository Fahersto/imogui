#include <functional>

namespace imogui
{
	class Renderer;
	class Steamoverlay
	{

	public:
		void Hook(std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}