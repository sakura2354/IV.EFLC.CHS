#pragma once

namespace Game
{
	void *Alloc(unsigned int);
	void Free(void *);

	int GetResolutionX();
	int GetResolutionY();

	void SetRenderState(void *, int = 1);
}
