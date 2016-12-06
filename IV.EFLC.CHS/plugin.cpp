#include <windows.h>
#include <Shlwapi.h>
#include <cstddef>
#include <cstring>
#include "deps/xliveless/xliveless.h"

#pragma comment(lib, "Shlwapi.lib")

namespace plugin
{
	char IVText[MAX_PATH];
	char EFLCText[MAX_PATH];
	char TBOGTText[MAX_PATH];
	char TLADText[MAX_PATH];

	bool CheckResource(HMODULE module)
	{
		char path[MAX_PATH];

		GetModuleFileNameA(module, path, MAX_PATH);

		std::strcpy(IVText, path);
		std::strcpy(EFLCText, path);
		std::strcpy(TBOGTText, path);
		std::strcpy(TLADText, path);

		std::ptrdiff_t index = std::strrchr(path, '.') - path;
		PathAppendA(IVText + index, "\\IV.CHS.gxt");
		PathAppendA(EFLCText + index, "\\EFLC.CHS.gxt");
		PathAppendA(TBOGTText + index, "\\TBOGT.CHS.gxt");
		PathAppendA(TLADText + index, "\\TLAD.CHS.gxt");

		if (dwGameVersion == IvPatch8)
		{
			return PathFileExistsA(IVText);
		}
		else if (dwGameVersion == EflcPatch3)
		{
			return (PathFileExistsA(EFLCText) && PathFileExistsA(TBOGTText) && PathFileExistsA(TLADText));
		}
		else
		{
			return TRUE;
		}
	}

	BOOL Patch()
	{
		switch (dwGameVersion)
		{
		case IvPatch8:
		{

			return TRUE;
		}

		case EflcPatch3:
		{
			return TRUE;
		}

		default:
		{
			return FALSE;
		}
		}
	}

	BOOL Init(HMODULE module)
	{
		if (CheckResource(module))
		{
			return Patch();
		}

		return FALSE;
	}

}
