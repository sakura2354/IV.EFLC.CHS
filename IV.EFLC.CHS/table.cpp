#include "table.h"
#include <array>
#include <memory>
#include <fstream>

static std::unique_ptr<std::array<CharPos, 0x10000>> pTable;

void CTable::read(const char *filename)
{
	std::ifstream input(filename, std::ios_base::binary);

	pTable = std::make_unique<std::array<CharPos, 0x10000>>();

	if (input)
	{
		input.read(reinterpret_cast<char *>(pTable->data()), 0x20000);
	}
	else
	{
		pTable->fill(CharPos{ 63,63 });
	}
}

const CharPos &CTable::get(wchar_t character)
{
	return pTable->data()[character];
}
