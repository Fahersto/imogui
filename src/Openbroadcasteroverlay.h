#include <functional>

namespace imogui
{
	class Renderer;
	class Openbroadcasteroverlay
	{

	public:
		/**
		* Inject into "NVIDIA Share.exe"
		*/
		void Hook(std::function<void(Renderer*)> drawCallback);
		void Unhook();
	};
}