#pragma once
#include <windows.h>
#include "deps/injector/injector.hpp"

namespace plugin
{
	void Init(HMODULE);

	enum class GameVersion
	{
		UNCHECKED,
		IV_1_0_8_0,
		EFLC_1_1_3_0,
		UNK
	};

	GameVersion GetGameVersion();

	injector::auto_pointer AddressByVersion(unsigned int addressiv, unsigned int addresseflc = 0);
};
