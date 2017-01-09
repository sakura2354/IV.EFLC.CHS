#include <windows.h>
#include <cstddef>
#include <cstring>

#include "plugin.h"
#include "font.h"
#include "dictionary.h"
#include "hash.h"
#include "table.h"
#include "deps/injector/hooking.hpp"
#include "deps/injector/assembly.hpp"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

static char TablePath[MAX_PATH];

static __declspec(naked) void Hook_GetStringWidth_100h()
{
	static void *retaddr;

	__asm //88A790 887A95
	{
		pop retaddr;
		movzx eax, word ptr[ebx];
		push eax;
		call CFont::IsNaiveCharacter;
		add esp, 4;
		test al, al;
		movzx eax, word ptr[ebx];
		mov cl, [esp + 0x10BC];
		jnz normal;
		jmp chs;

	normal:
		mov edx, retaddr;
		add edx, 0x2;
		jmp edx;

	chs:
		test cl, cl;
		jnz normal;
		mov dl, [esp + 0x13];
		test dl, dl;
		jz normal;
		mov edx, retaddr;
		add edx, 0x274;
		jmp edx;
	}
}


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
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x100), Hook_GetStringWidth_100h);

		injector::MakeJMP(AddressByVersion(0x8859E0), CFont::SkipAWord);

		injector::MakeCALL(AddressByVersion(0x884A90 + 0xD4), CFont::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x145), CFont::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x8D32D0 + 0x535), CFont::GetCharacterSizeNormalDispatch);

		injector::MakeCALL(AddressByVersion(0x8843E0 + 0x45), CFont::GetCharacterSizeDrawingDispatch);
		injector::MakeCALL(AddressByVersion(0x8843E0 + 0x3B5), CFont::GetCharacterSizeDrawingDispatch);
		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x14A), CFont::GetCharacterSizeDrawingDispatch);

		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x142), CFont::PrintCharDispatch);
		injector::MakeCALL(AddressByVersion(0x88A010 + 0x490), CFont::PrintCharDispatch);

		injector::MakeCALL(AddressByVersion(0x887642), CFont::LoadTextureCB);
		injector::MakeCALL(AddressByVersion(0x887CB6), CFont::LoadTextureCB);

		injector::WriteMemory(plugin::AddressByVersion(0x8A38B4), "CHINESE.GXT", true);
		injector::WriteMemory(plugin::AddressByVersion(0x8A38DC), "CHINESE.GXT", true);

		injector::WriteMemory(plugin::AddressByVersion(0x8874FC), "platform:/textures/fonts_cn", true);
		injector::WriteMemory(plugin::AddressByVersion(0x887B73), "platform:/textures/fonts_cn", true);
	}

	void Init(HMODULE module)
	{
		GetModuleFileNameA(module, TablePath, MAX_PATH);

		std::strcpy(std::strrchr(TablePath, '\\'), "\\wmhhz\\table.dat");

		CTable::read(TablePath);

		Patch();
	}
}

