#include "game.h"
#include "plugin.h"
#include "deps/injector/calling.hpp"

namespace Game
{
	void *Alloc(unsigned int arg)
	{
		return injector::cstd<unsigned int *(unsigned int)>::call(plugin::AddressByVersion(0x4011D0), arg);
	}

	void Free(void *arg)
	{
		injector::cstd<void(void *)>::call(plugin::AddressByVersion(0x401210), arg);
	}

	int GetResolutionX()
	{
		return injector::cstd<int()>::call(plugin::AddressByVersion(0x4035A0));
	}

	int GetResolutionY()
	{
		return injector::cstd<int()>::call(plugin::AddressByVersion(0x4035C0));
	}

	void SetRenderState(void *pTexture, int arg_4)
	{
		injector::cstd<void(void*, int)>::call(plugin::AddressByVersion(0x85CF60), pTexture, arg_4);
	}
}
