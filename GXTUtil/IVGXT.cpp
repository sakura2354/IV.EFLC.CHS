#include "IVGXT.h"
#include "uint32_hex.h"
#include "binary_fstream.hpp"

#include <cstring>
#include <iostream>
#include <array>
#include <regex>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <iterator>
#include <memory>

using namespace std;

bool IVGXT::has_ext_txt(const string &name)
{
	auto endit = name.end();

	return (name.length() > 3 &&
		(*(endit - 4) == '.') &&
		(*(endit - 3) == 't' || *(endit - 3) == 'T') &&
		(*(endit - 2) == 'x' || *(endit - 2) == 'X') &&
		(*(endit - 1) == 't' || *(endit - 1) == 'T'));
}

bool IVGXT::has_ext_gxt(const string &name)
{
	auto endit = name.end();

	return (name.length() > 3 &&
		(*(endit - 4) == '.') &&
		(*(endit - 3) == 'g' || *(endit - 3) == 'G') &&
		(*(endit - 2) == 'x' || *(endit - 2) == 'X') &&
		(*(endit - 1) == 't' || *(endit - 1) == 'T'));
}

void IVGXT::load_text(const string &text_path)
{
	ifstream text_stream(text_path);

	regex table_format(R"-(\[([0-9A-Za-z_]{1,7})\])-");
	regex entry_format(R"-((0[xX][0-9a-fA-F]{8})=(.+))-");

	smatch match_result;

	string table_name;

	uint32_t hash;
	array<char, 11> hash_string;
	
	string normal_text;
	wstring widen_text;

	map<string, vector<pair<uint32_t, pair<string, wstring>>>, TABLE_SORT>::iterator table_iter;

	string line_buffer;

	size_t line_no = 0;

	if (!text_stream)
	{
		return;
	}

	m_data.clear();
	m_collection.clear();

	table_iter = m_data.end();

	skip_utf8_signature(text_stream);

	while (getline(text_stream, line_buffer))
	{
		++line_no;

		if (line_buffer.empty() || line_buffer.front() == ';')
		{
			continue;
		}

		if (regex_match(line_buffer, match_result, entry_format))
		{
			if (table_iter == m_data.end())
			{
				return;
			}

			hash_string.fill(0);
			copy(match_result[1].first, match_result[1].second, hash_string.begin());

			uint32_hex::from_hex(hash_string, hash);

			normal_text.assign(match_result[2].first, match_result[2].second);

			widen_text = to_wide(normal_text);

			collect_wide_chars(widen_text);

			transform_characters(widen_text);

			table_iter->second.push_back(make_pair(hash, make_pair(normal_text, widen_text)));
		}
		else if (regex_match(line_buffer, match_result, table_format))
		{
			table_name.assign(match_result[1].first, match_result[1].second);

			table_iter = m_data.insert(make_pair(table_name, vector<pair<uint32_t, pair<string, wstring>>>())).first;
		}
		else
		{
			return;
		}
	}
}

void IVGXT::load_binary(const string &binary_path)
{
	GXT_HEADER gxtheader;
	TABL_HEADER tablheader;
	TKEY_HEADER_MAIN tkeyheadermain;
	TKEY_HEADER tkeyheader;
	TDAT_HEADER tdatheader;

	vector<TABL_ENTRY> tabl_block;
	vector<TKEY_ENTRY>tkey_block;
	vector<char> tdat_block;

	vector<pair<uint32_t, pair<string, wstring>>> temp_table_data;

	wstring raw_text;
	string plain_text;

	binary_fstream input_file(binary_path);

	if (!input_file)
	{
		return;
	}

	m_data.clear();

	input_file.read(gxtheader);

	if (!gxtheader.valid())
	{
		return;
	}

	input_file.read(tablheader);

	if (!tablheader.valid())
	{
		return;
	}

	tabl_block.resize(tablheader.count());

	input_file.read((char *)tabl_block.data(), tablheader.size());

	for (auto &tablentry : tabl_block)
	{
		input_file.seekg(tablentry.m_offset);

		if (strncmp(tablentry.m_name, "MAIN", 5) == 0)
		{
			input_file.read(tkeyheadermain);

			if (!tkeyheadermain.valid())
			{
				return;
			}

			tkey_block.resize(tkeyheadermain.count());

			input_file.read((char *)tkey_block.data(), tkeyheadermain.size());

		}
		else
		{
			input_file.read(tkeyheader);

			if (!tkeyheader.valid(tablentry.m_name))
			{
				return;
			}

			tkey_block.resize(tkeyheader.count());

			input_file.read((char *)tkey_block.data(), tkeyheader.size());
		}

		input_file.read(tdatheader);

		if (!tdatheader.valid())
		{
			return;
		}

		tdat_block.resize(tdatheader.size());

		input_file.read(tdat_block.data(), tdatheader.size());

		temp_table_data.clear();

		for (auto &tkeyentry : tkey_block)
		{
			raw_text = (wchar_t *)(tdat_block.data() + tkeyentry.m_offset);

			transform_characters(raw_text);

			plain_text = to_narrow(raw_text);

			temp_table_data.push_back(make_pair(tkeyentry.m_hash, make_pair(plain_text, raw_text)));
		}

		m_data[tablentry.m_name] = temp_table_data;
	}
}

void IVGXT::generate_text()
{
	array<char, 11> hex_string;

	ofstream text_stream;

	for (auto &table : m_data)
	{
		text_stream.open(string("C:\\TEXT\\OUTPUT\\") + table.first.data() + ".txt", ios::trunc);

		if (!text_stream)
		{
			return;
		}

		add_utf8_signature(text_stream);

		text_stream << '[' << table.first.data() << ']' << '\n';

		for (auto &entry : table.second)
		{
			uint32_hex::to_hex(entry.first, hex_string);

			text_stream << ';' << hex_string.data() << '=' << entry.second.first << '\n';
			text_stream << hex_string.data() << '=' << entry.second.first << '\n' << '\n';
		}

		text_stream.close();
	}
}

void IVGXT::generate_binary(const string &binary_path)
{
	int32_t foTableBlock, foKeyBlock, foDataBlock;
	int32_t tableBlockSize, keyBlockOffset, keyBlockSize, TDATOffset, dataOffset, dataBlockSize;

	tableBlockSize = this->m_data.size() * 12;

	binary_fstream binary_stream(binary_path, ios::binary | ios::out | ios::trunc);

	if (!binary_stream)
	{
		return;
	}

	binary_stream.write("\x04\x00\x10\x00", 4);
	binary_stream.write("TABL", 4);
	binary_stream.write(tableBlockSize);

	foTableBlock = 12;
	foKeyBlock = tableBlockSize + 12;
	keyBlockOffset = foKeyBlock;

	for (auto &table : this->m_data)
	{
		keyBlockSize = table.second.size() * 8;
		dataBlockSize = get_data_size(table.second);
		binary_stream.seekp(foTableBlock);
		binary_stream.write_container(table.first);
		binary_stream.write(keyBlockOffset);
		foTableBlock += 12;
		binary_stream.seekp(foKeyBlock);

		if (!equal(table.first.begin(), table.first.begin() + 5, "MAIN"))
		{
			binary_stream.write_container(table.first);
		}

		binary_stream.write("TKEY", 4);
		binary_stream.write(keyBlockSize);
		foKeyBlock = binary_stream.tellp();
		binary_stream.seekp(keyBlockSize, ios::cur);
		TDATOffset = binary_stream.tellp();
		binary_stream.write("TDAT", 4);
		binary_stream.write(dataBlockSize);
		foDataBlock = binary_stream.tellp();

		for (auto &entry : table.second)
		{
			dataOffset = foDataBlock - TDATOffset - 8;
			binary_stream.seekp(foKeyBlock);
			binary_stream.write(dataOffset);
			binary_stream.write(entry.first);
			foKeyBlock += 8;
			binary_stream.seekp(foDataBlock);
			binary_stream.write_container(entry.second.second);
			binary_stream.write(0ui16);
			foDataBlock = binary_stream.tellp();
		}

		foKeyBlock = binary_stream.tellp();
		keyBlockOffset = foKeyBlock;
	}
}

wstring IVGXT::to_wide(const string &input)
{
	return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(input);
}

string IVGXT::to_narrow(const wstring &input)
{
	return wstring_convert<codecvt_utf8<wchar_t>>().to_bytes(input);
}

size_t IVGXT::get_data_size(const vector<pair<uint32_t, pair<string, wstring>>> &table)
{
	size_t result = 0;

	for (auto &key : table)
	{
		result += (key.second.second.size() * 2 + 2);
	}

	return result;
}

void IVGXT::skip_utf8_signature(ifstream &text_stream)
{
	char header[3];

	text_stream.seekg(0);

	if (text_stream.get(header[0]) && text_stream.get(header[1]) && text_stream.get(header[2]))
	{
		if (header[0] == '\xEF' && header[1] == '\xBB' && header[2] == '\xBF')
		{
			return;
		}
	}

	text_stream.seekg(0);
}

void IVGXT::add_utf8_signature(ofstream &text_stream)
{
	text_stream << "\xEF\xBB\xBF";
}

void IVGXT::txt2gxt(const string &input_path, const string &output_path)
{
	load_text(input_path);
	generate_binary(output_path);
}

void IVGXT::gxt2txt(const string &input_path)
{
	load_binary(input_path);
	generate_text();
}

void IVGXT::process(const string &input_path, const string &output_path)
{
	bool is_input_text = has_ext_txt(input_path);
	bool is_output_text = has_ext_txt(output_path);
	bool is_input_binary = has_ext_gxt(input_path);
	bool is_output_binary = has_ext_gxt(output_path);

	if (is_input_text && is_output_binary)
	{
		txt2gxt(input_path, output_path);
	}
	else if (is_input_binary && is_output_text)
	{
		gxt2txt(input_path);
	}
}

void IVGXT::collect_wide_chars(const wstring &wide_text)
{
	for (wchar_t wchar : wide_text)
	{
		if (wchar < 0x80 ||

			wchar == 0xA9 ||
			wchar == 0xAC ||
			wchar == 0xAE ||
			wchar == 0xC1 ||
			wchar == 0xC9 ||
			wchar == 0xD6 ||
			wchar == 0xDC ||
			wchar == 0xE0 ||
			wchar == 0xE1 ||
			wchar == 0xE7 ||
			wchar == 0xE8 ||
			wchar == 0xE9 ||
			wchar == 0xEA ||
			wchar == 0xED ||
			wchar == 0xEF ||
			wchar == 0xF1 ||
			wchar == 0xF3 ||
			wchar == 0xF5 ||
			wchar == 0xFA ||
			wchar == 0xFC ||
			wchar == 0x2122)
		{
			return;
		}

		m_collection.insert(wchar);
	}
}

void IVGXT::transform_characters(vector<wchar_t> &input)
{
	//bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
	//bad character in EFLC stock text: 0x93

	for (wchar_t &character : input)
	{
		switch (character)
		{

		case 0x85:
			character = L' ';
			break;

		case 0x92:
		case 0x94:
			character = L'\'';
			break;

		case 0x93: //EFLC
			break;

		case 0x96:
			character = L'-';
			break;

		case 0x97:
		case 0xA0:
			character = L' ';
			break;

		case 0x99:
			character = L'™';
			break;



		default:
			break;
		}
	}
}

void IVGXT::transform_characters(wstring &input)
{
	//bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
	//bad character in EFLC stock text: 0x93

	for (wchar_t &character : input)
	{
		if (character == L'™')
			character = 0x99;
	}
}

void IVGXT::generate_collection(const string &collection_path)
{
	binary_fstream output_stream(collection_path, ios::binary | ios::out);

	if (!output_stream)
	{
		return;
	}

	unsigned char curcolumn = 0;

	output_stream.write(0xFEFFui16);

	for (wchar_t character : this->m_collection)
	{
		output_stream.write(character);

		curcolumn += 1;

		if (curcolumn > 62)
		{
			output_stream.write(L'\n');

			curcolumn = 0;
		}
	}
}

void IVGXT::generate_table(const string &table_path)
{
	struct CharPos
	{
		unsigned char row, column;
	};

	unsigned char currow = 0;
	unsigned char curcolumn = 0;

	binary_fstream output_stream(table_path, ios::binary | ios::out);

	if (!output_stream)
	{
		return;
	}

	unique_ptr<array<CharPos, 0x10000>> pTable = make_unique<array<CharPos, 0x10000>>();

	pTable->fill(CharPos{ 50, 63 });

	for (wchar_t character : this->m_collection)
	{
		pTable->data()[character].row = currow;
		pTable->data()[character].column = curcolumn;

		if (curcolumn < 63)
		{
			++curcolumn;
		}
		else
		{
			++currow;

			curcolumn = 0;
		}
	}

	output_stream.write_container(*pTable);
}
