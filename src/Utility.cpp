#include "Utility.h"

#include <Windows.h>
#include <vector>

namespace imogui
{
	int8_t* Utility::Scan(const char* module, const char* signature)
	{
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		int8_t* moduleBaseAddress = (int8_t*)GetModuleHandleA(module);
		auto dosHeader = (PIMAGE_DOS_HEADER)moduleBaseAddress;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleBaseAddress + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);
		int8_t* scanBytes = moduleBaseAddress;

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;

			if (i == 0x890E0)
			{
				printf("Test\n");
			}

			for (auto j = 0ul; j < s; ++j) {
				int8_t currentByte = scanBytes[i + j];
				int8_t currentByteInPattern = d[j];

				if (currentByte != currentByteInPattern && currentByteInPattern != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	}
}
