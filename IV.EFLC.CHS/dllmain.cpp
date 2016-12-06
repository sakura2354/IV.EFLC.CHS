#include <windows.h>
#include "plugin.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		return plugin::Init(module);
	}

	return TRUE;
}
