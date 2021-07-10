#include <functional>

namespace imogui
{
	class Renderer;
	class Discordoverlay
	{

	public:
		void Hook(std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}