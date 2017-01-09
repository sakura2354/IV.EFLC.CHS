#include <windows.h>
#include <cstddef>
#include <cstring>

#include "plugin.h"
#include "font.h"
#include "dictionary.h"
#include "hash.h"
#include "table.h"
#include "deps/injector/hooking.hpp"

static char TablePath[MAX_PATH];

namespace plugin
{
	GameVersion GetGameVersion()
	{
		static GameVersion product = GameVersion::UNCHECKED;

		if (product == GameVersion::UNCHECKED)
		{
			switch (injector::ReadMemory<unsigned int>(injector::aslr_ptr(0x608C34).get(), true))
			{
			case 0x404B100F:
				product = GameVersion::IV_1_0_8_0;
				break;

			case 0x04C1F600:
				product = GameVersion::EFLC_1_1_3_0;
				break;

			default:
				product = GameVersion::UNK;
				break;
			}
		}

		return product;
	}

	injector::auto_pointer AddressByVersion(unsigned int addressiv, unsigned int addresseflc)
	{
		switch (GetGameVersion())
		{
		case GameVersion::IV_1_0_8_0:
			return injector::aslr_ptr(addressiv).get();

		case GameVersion::EFLC_1_1_3_0:
			return injector::aslr_ptr(addresseflc).get();

		default:
			return injector::auto_pointer();
		}

	}

	void Patch()
	{
#if 0
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x100), Font::GetStringWidthHook);

		injector::MakeJMP(AddressByVersion(0x8859E0), Font::SkipAWord);

		injector::MakeCALL(AddressByVersion(0x884A90 + 0xD4), Font::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x145), Font::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x8D32D0 + 0x535), Font::GetCharacterSizeNormalDispatch);

		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x14A), Font::GetCharacterSizeDrawingDispatch);
		injector::MakeCALL(AddressByVersion(0x88A58B), Font::GetCharacterSizeDrawingDispatch);

		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x142), Font::PrintCharDispatch);
		injector::MakeCALL(AddressByVersion(0x88A010 + 0x490), Font::PrintCharDispatch);

		injector::MakeCALL(AddressByVersion(0x887642), Font::LoadTextureCB);
		injector::MakeCALL(AddressByVersion(0x887CB6), Font::LoadTextureCB);

		injector::WriteMemory(plugin::AddressByVersion(0x8A38B4), "CHINESE.GXT", true);
		injector::WriteMemory(plugin::AddressByVersion(0x8A38DC), "CHINESE.GXT", true);

		injector::WriteMemory(plugin::AddressByVersion(0x8874FC), "platform:/textures/fonts_cn", true);
		injector::WriteMemory(plugin::AddressByVersion(0x887B73), "platform:/textures/fonts_cn", true);
#endif
	}

	void Init(HMODULE module)
	{
		GetModuleFileNameA(module, TablePath, MAX_PATH);

		std::strcpy(std::strrchr(TablePath, '\\'), "\\wmhhz\\table.dat");

		CTable::read(TablePath);

		Patch();
	}
}

