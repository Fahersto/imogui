#include <functional>

namespace imogui
{
	class Renderer;
	class Originoverlay
	{

	public:
		void Hook(std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}