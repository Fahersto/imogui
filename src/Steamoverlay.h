#include <functional>

namespace imogui
{
	class Steamoverlay
	{
	public:
		void Hook(std::function<void()> drawCallback);
	};
}