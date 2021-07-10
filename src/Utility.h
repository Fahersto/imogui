#include <cstdint>

namespace imogui
{
	class Utility
	{

	public:
		static int8_t* ScanBytePattern(const char* module, const char* signature);
	};
}