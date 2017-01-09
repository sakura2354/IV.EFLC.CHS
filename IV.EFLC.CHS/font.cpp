#include "plugin.h"
#include "font.h"
#include "game.h"
#include "dictionary.h"
#include "hash.h"
#include "table.h"
#include "deps/injector/calling.hpp"
#include <cstring>

static const float fChsWidth = 28.0f;
static const float fTextureResolution = 4096.0f;
static const float fSpriteSize = 64.0f;

static CFontDetails *pDetails = plugin::AddressByVersion(0x11F5BC8);
static CFontRenderState *pRenderState = plugin::AddressByVersion(0xF38114);
static CFontInfo *pFonts= plugin::AddressByVersion(0x11EC930);
static float *pFontResX= plugin::AddressByVersion(0xF3814C);

static void *pTexture;

unsigned char CFont::GetRenderIndex()
{
	return injector::cstd<unsigned char()>::call(plugin::AddressByVersion(0x8840B0));
}

bool CFont::IsNaiveCharacter(wchar_t character)
{
	return (
		character < 0x80 ||
		character == 0xA9 ||
		character == 0xAC ||
		character == 0xAE ||
		character == 0xC1 ||
		character == 0xC9 ||
		character == 0xD6 ||
		character == 0xDC ||
		character == 0xE0 ||
		character == 0xE1 ||
		character == 0xE7 ||
		character == 0xE8 ||
		character == 0xE9 ||
		character == 0xEA ||
		character == 0xED ||
		character == 0xEF ||
		character == 0xF1 ||
		character == 0xF3 ||
		character == 0xF5 ||
		character == 0xFA ||
		character == 0xFC ||
		character == 0x99);
}

void * CFont::LoadTextureCB(void *pDictionary, int, unsigned int hash)
{
	void *result = Dictionary::GetElementByKey(pDictionary, hash);

	pTexture = Dictionary::GetElementByKey(pDictionary, Hash::HashStringFromSeediCase("chs"));

	return result;
}

wchar_t *CFont::SkipAWord(wchar_t *text)
{
	if (!text)
	{
		return text;
	}

	wchar_t *begin = text;
	wchar_t *current = text;

	while (true)
	{
		wchar_t character = *current;

		if (character == ' ' || character == '~' || character == 0)
		{
			break;
		}

		if (!IsNaiveCharacter(character))
		{
			if (current == begin)
			{
				++current;
			}
			
			break;
		}
		else
		{
			++current;
		}
	}

	return current;
}
float CFont::GetCHSCharacterSizeNormal()
{
	unsigned char index = GetRenderIndex();

	float extrawidth = pFonts[pDetails[index].nFont].fWidthOfSpaceBetweenChars[pDetails[index].nExtraWidthIndex];

	return (((fChsWidth + extrawidth) / *pFontResX + pDetails[index].fEdgeSize2) * pDetails[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(wchar_t character)
{
	if (IsNaiveCharacter(character + 0x20))
	{
		return injector::cstd<float(wchar_t)>::call(plugin::AddressByVersion(0x884110), character);
	}
	else
	{
		return GetCHSCharacterSizeNormal();
	}
}

float CFont::GetCHSCharacterDrawing(bool useextrawidth)
{
	unsigned char index = GetRenderIndex();

	float extrawidth = useextrawidth ? pFonts[pRenderState->nFont].fWidthOfSpaceBetweenChars[pRenderState->nExtraWidthIndex] : 0.0f;

	return (((fChsWidth + useextrawidth) / *pFontResX + pRenderState->fEdgeSize) * pRenderState->fScaleX);
}

float CFont::GetCharacterSizeDrawingDispatch(wchar_t character, bool useextrawidth)
{
	if (IsNaiveCharacter(character + 0x20))
	{
		return injector::cstd<float(wchar_t, bool)>::call(plugin::AddressByVersion(0x874040), character, useextrawidth);
	}
	else
	{
		return GetCHSCharacterDrawing(useextrawidth);
	}
}

void CFont::PrintCHSChar(float posx, float posy, wchar_t character)
{
	rageRect screenrect, texturerect;

	if (posy < -0.06558f || posy > 1.0f)
	{
		return;
	}

	if (-(GetCHSCharacterDrawing(true) / pRenderState->fScaleX) > posx || posx > 1.0f)
	{
		return;
	}

	CharPos charpos = CTable::get(character);

	float rawwidth = fChsWidth / fTextureResolution;
	//8848ee
	Game::SetRenderState(pTexture);
	injector::cstd<void(rageRect*, rageRect*, rageRGBA, int)>::call(plugin::AddressByVersion(0x884300), &screenrect, &texturerect, pRenderState->field_18, 0);
}

void CFont::PrintCharDispatch(float posx, float posy, wchar_t character, int mode)
{
	if (IsNaiveCharacter(character + 0x20))
	{
		injector::cstd<void(float, float, wchar_t, int)>::call(plugin::AddressByVersion(0x8843E0), posx, posy, character, mode);
	}
	else
	{
		PrintCHSChar(posx, posy, character + 0x20);
	}
}
