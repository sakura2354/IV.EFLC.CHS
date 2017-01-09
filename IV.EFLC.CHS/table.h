#pragma once

struct CharPos
{
	unsigned char row, column;
};

class CTable
{
public:
	static void read(const char *);
	static const CharPos &get(wchar_t);
};

