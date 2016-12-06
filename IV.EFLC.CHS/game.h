#pragma once

#define VALIDATE_SIZE(expr, size, hint) static_assert(sizeof(expr) == size, hint);

struct rageRGBA
{
	unsigned char red, green, blue, alpha;
};

struct rageRect
{
	float field_0;
	float field_4;
	float field_8;
	float field_C;
};

struct TextSetting
{
	int nWideScreenFormat;
	float fScaleX;
	float fScaleY;
	int nViewportRelated;
	bool bLineDisplayFlag1;
	bool bLineDisplayFlag2;
	char pad1[2];
	rageRGBA Colour;
	bool bJustify;
	bool bRightJustify;
	bool bCentre;
	bool bProportional;
	bool bUseUnderscore;
	bool bBackground;
	bool bUnused;
	char pad2[1];
	float fWrapX;
	float fCentreWrapX;
	float fLineHeightZMultiplier;
	bool bUseTokenColours;
	char pad3[3];
	rageRGBA Unknown;
	rageRGBA FXColour;
	bool bDropShadow;
	bool bEdge;
	bool bDrawBeforeFade;
	char pad4[1];
	int nFont;
	float fPosX;
	float fPosY;
	char field_48[16];
	char *pSubstr1;
	char *pSubstr2;
	int nNumber1;
	int nNumber2;
	int nNumber3;
	float fNumber;
	int field_70;
	int field_74;
	int field_78;
};
VALIDATE_SIZE(TextSetting, 0x7C, "hehe")

struct g_TextSettings
{
	TextSetting dummy;
	TextSetting settings[256];
};

struct size_0x48
{
	int field_0;
	float field_4;
	float field_8;
	float field_C;
	int field_10;
	bool field_14;
	bool field_15;
	bool field_16;
	bool field_17;
	bool field_18;
	bool field_19;
	bool field_1A;
	char pad1[1];
	int field_1C;
	float field_20;
	float field_24;
	bool field_28;
	bool field_29;
	char pad2[2];
	float field_2C;
	int field_30;
	float field_34;
	float field_38;
	float field_3C;
	float field_40;
	bool field_44;
	char pad3[3];
};
VALIDATE_SIZE(size_0x48, 0x48, "hehe")

struct g_size_0x48
{
	size_0x48 size_0x48s[2];
};

struct font_info
{
	unsigned char iPropValues[255];
	unsigned char iTextureMap[255];
	char pad1[2];
	float fUnpropValue;
	float fWidthOfSpaceBetweenChars1;
	float fWidthOfSpaceBetweenChars2;
	float fWidthOfSpaceBetweenChars3;
	char pad2[8];
	float fJapaneseSubFont1SpaceWidth;
	float fJapaneseSubFont2SpaceWidth;
	int iMainFontStart;
	int iMainFontEnd;
	int iSubFont1Start;
	int iSubFont1End;
	int iSubFont2Start;
	int iSubFont2End;
	int iCommonFontStart;
	int iCommonFontEnd;
	void *pTexture;
	float fTextureWidth;
	float fTextureHeight;
	float fSpriteSize;
	float field_250;
	float field_254;
};
VALIDATE_SIZE(font_info, 0x258, "hehe")

union g_font_infos
{
	font_info font_infos[4];

	struct
	{
		font_info font1;
		font_info font3;
		font_info font4;
		font_info font2;
	} individual;
};

struct size_0x38
{
	int field_0;
	float field_4;
	float field_8;
	float field_C;
	float field_10;
	float field_14;
	rageRGBA field_18;
	float field_1C;
	int field_20;
	bool field_24;
	bool field_25;
	bool field_26;
	bool field_27;
	bool field_28;
	bool field_29;
	char pad1[2];
	float field_2C;
	float field_30;
	bool field_34;
	char pad2[3];
};
VALIDATE_SIZE(size_0x38, 0x38, "hehe")

struct CStringProcessResult
{
	void *pvtbl;
	int nNumLines;
	float fSpaceWidth;

	CStringProcessResult()
	{
		pvtbl = nullptr;
		nNumLines = 0;
		fSpaceWidth = 0.0f;
	}
};
VALIDATE_SIZE(CStringProcessResult, 0xC, "hehe")
