#include "game.h"
#include "plugin.h"
#include "deps/injector/calling.hpp"

namespace Game
{
	void SetRenderState(void *pTexture, int arg_4)
	{
		injector::cstd<void(void*, int)>::call(plugin::AddressByVersion(0x85CF60), pTexture, arg_4);
	}
}
