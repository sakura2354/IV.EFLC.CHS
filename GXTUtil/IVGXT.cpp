#include "IVGXT.h"
#include "uint32_hex.h"
#include "print.hpp"
#include "CBinaryStream.hpp"

#include <cstring>
#include <iostream>
#include <array>
#include <regex>
#include <locale>
#include <codecvt>
#include <algorithm>

bool IVGXT::table_sort::operator()(const std::array<char, 8> &lhs, const std::array<char, 8> &rhs) const
{
	if (std::equal(rhs.begin(), rhs.begin() + 5, "MAIN"))
	{
		return false;
	}
	else if (std::equal(lhs.begin(), lhs.begin() + 5, "MAIN"))
	{
		return true;
	}
	else
	{
		return (std::strcmp(lhs.data(), rhs.data()) < 0);
	}
}

bool IVGXT::has_ext_txt(const std::string &name)
{
	auto endit = name.end();

	return (name.length() > 3 &&
		(*(endit - 4) == '.') &&
		(*(endit - 3) == 't' || *(endit - 3) == 'T') &&
		(*(endit - 2) == 'x' || *(endit - 2) == 'X') &&
		(*(endit - 1) == 't' || *(endit - 1) == 'T'));
}

bool IVGXT::has_ext_gxt(const std::string &name)
{
	auto endit = name.end();

	return (name.length() > 3 &&
		(*(endit - 4) == '.') &&
		(*(endit - 3) == 'g' || *(endit - 3) == 'G') &&
		(*(endit - 2) == 'x' || *(endit - 2) == 'X') &&
		(*(endit - 1) == 't' || *(endit - 1) == 'T'));
}

void IVGXT::load_text(const std::string &text_path)
{
	std::ifstream text_stream(text_path);

	std::regex table_format(R"-(\[([0-9A-Za-z_]{1,7})\])-");
	std::regex entry_format(R"-((0[xX][0-9a-fA-F]{8})=(.+))-");

	std::smatch match_result;

	std::array<char, 8> table_name;

	std::uint32_t hash;
	std::array<char, 11> hash_string;
	
	std::string normal_text;
	std::wstring widen_text;

	std::map<std::array<char, 8>, std::map<std::uint32_t, std::pair<std::string, std::wstring> >, table_sort>::iterator table_iter;

	std::string line_buffer;

	std::size_t line_no = 0;

	if (!text_stream)
	{
		return;
	}

	m_data.clear();
	m_collection.clear();

	table_iter = m_data.end();

	skip_utf8_signature(text_stream);

	while (std::getline(text_stream, line_buffer))
	{
		++line_no;

		if (line_buffer.empty() || line_buffer.front() == ';')
		{
			continue;
		}

		if (std::regex_match(line_buffer, match_result, entry_format))
		{
			if (table_iter == m_data.end())
			{
				return;
			}

			hash_string.fill(0);
			std::copy(match_result[1].first, match_result[1].second, hash_string.begin());

			if (uint32_hex::from_hex(hash_string, hash))
			{
				normal_text.assign(match_result[2].first, match_result[2].second);

				normal_text.push_back(0);

				to_wide(normal_text, widen_text);

				collect_wide_chars(widen_text);

				table_iter->second.insert(std::make_pair(hash, std::make_pair(normal_text, widen_text)));
			}
			else
			{
				return;
			}
		}
		else if (std::regex_match(line_buffer, match_result, table_format))
		{
			table_name.fill(0);

			std::copy(match_result[1].first, match_result[1].second, table_name.begin());

			table_iter = m_data.insert(std::make_pair(table_name, std::map<std::uint32_t, std::pair<std::string, std::wstring> >())).first;
		}
		else
		{
			return;
		}
	}
}

void IVGXT::load_binary(const std::string &binary_path)
{
	std::int32_t tables_count, keys_count, offset_of_beginning_of_keys, offset_of_beginning_of_data;

	TABL temp_tabl;
	TKEY temp_tkey;

	std::pair<std::array<char, 8>, std::map<std::uint32_t, std::pair<std::string, std::wstring> > > temp_table;

	std::wstring wide_text;
	std::string narrow_text;

	CBinaryStream binary_stream(binary_path, std::ios_base::in | std::ios_base::binary);

	if (!binary_stream.is_open())
	{
		return;
	}

	m_data.clear();
	m_collection.clear();

	binary_stream.seekg(8);
	binary_stream.read(tables_count);
	tables_count /= 12;

	for (std::int32_t table_index = 0; table_index < tables_count; ++table_index)
	{
		binary_stream.seekg(12 + table_index * 12);
		binary_stream.read(temp_tabl);
		std::copy(std::begin(temp_tabl.name), std::end(temp_tabl.name), temp_table.first.begin());

		binary_stream.seekg(temp_tabl.offset);

		if (!std::equal(temp_tabl.name, temp_tabl.name + 5, "MAIN"))
		{
			binary_stream.seekg(8, std::ios_base::cur);
		}

		binary_stream.seekg(4, std::ios_base::cur);
		binary_stream.read(keys_count);
		offset_of_beginning_of_keys = binary_stream.tellg();
		offset_of_beginning_of_data = offset_of_beginning_of_keys + keys_count + 4 + 4;
		keys_count /= 8;

		for (std::int32_t key_index = 0; key_index < keys_count; ++key_index)
		{
			binary_stream.seekg(offset_of_beginning_of_keys + key_index * 8);
			binary_stream.read(temp_tkey);

			binary_stream.seekg(offset_of_beginning_of_data + temp_tkey.offset);

			read_c_wstring(binary_stream, wide_text);
			std::replace(wide_text.begin(), wide_text.end(), 0x85, 0x20);

			to_narrow(wide_text, narrow_text);

			temp_table.second.insert(std::make_pair(temp_tkey.hash, std::make_pair(narrow_text, wide_text)));
		}

		m_data.insert(temp_table);
		temp_table.second.clear();
	}
}

void IVGXT::generate_text(const std::string &text_path)
{
	std::array<char, 11> hex_string;

	std::ofstream text_stream(text_path, std::ios_base::trunc);

	if (!text_stream)
	{
		return;
	}

	add_utf8_signature(text_stream);

	for (auto &table : m_data)
	{
		text_stream << '[' << table.first.data() << ']' << '\n';

		for (auto &entry : table.second)
		{
			uint32_hex::to_hex(entry.first, hex_string);

			text_stream << ';' << hex_string.data() << '=' << entry.second.first << '\n';
			text_stream << hex_string.data() << '=' << entry.second.first << '\n' << '\n';
		}
	}
}

void IVGXT::generate_binary(const std::string &binary_path)
{
	std::int32_t foTableBlock, foKeyBlock, foDataBlock;
	std::int32_t tableBlockSize, keyBlockOffset, keyBlockSize, TDATOffset, dataOffset, dataBlockSize;

	tableBlockSize = this->m_data.size() * 12;

	CBinaryStream binary_stream(binary_path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

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

		if (!std::equal(table.first.begin(), table.first.begin() + 5, "MAIN"))
		{
			binary_stream.write_container(table.first);
		}

		binary_stream.write("TKEY", 4);
		binary_stream.write(keyBlockSize);
		foKeyBlock = binary_stream.tellp();
		binary_stream.seekp(keyBlockSize, std::ios_base::cur);
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
			foDataBlock = binary_stream.tellp();
		}

		foKeyBlock = binary_stream.tellp();
		keyBlockOffset = foKeyBlock;
	}
}

void IVGXT::to_wide(const std::string &input, std::wstring &output)
{
	output = std::wstring_convert<std::codecvt_utf8<wchar_t> >().from_bytes(input);
	output.push_back(0);
}

void IVGXT::to_narrow(const std::wstring &input, std::string &output)
{
	output = std::wstring_convert<std::codecvt_utf8<wchar_t> >().to_bytes(input);
}

std::size_t IVGXT::get_data_size(const std::map<std::uint32_t, std::pair<std::string, std::wstring> > &table)
{
	std::size_t result = 0;

	for (auto &key : table)
	{
		result += (key.second.second.size() * 2 + 2);
	}

	return result;
}

void IVGXT::skip_utf8_signature(std::ifstream &text_stream)
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

void IVGXT::add_utf8_signature(std::ofstream &text_stream)
{
	text_stream << "\xEF\xBB\xBF";
}

void IVGXT::txt2gxt(const std::string &input_path, const std::string &output_path)
{
	load_text(input_path);
	generate_binary(output_path);
}

void IVGXT::gxt2txt(const std::string &input_path, const std::string &output_path)
{
	load_binary(input_path);
	generate_text(output_path);
}

void IVGXT::process(const std::string &input_path, const std::string &output_path)
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
		gxt2txt(input_path, output_path);
	}
	else
	{
		print_to_cout("本工具只能 gxt<-->txt");
	}
}

void IVGXT::read_c_wstring(CBinaryStream &binary_stream, std::wstring &output)
{
	wchar_t meta;

	output.clear();

	binary_stream.read(meta);

	while (meta != 0)
	{
		output.push_back(meta);
		binary_stream.read(meta);
	}
}

void IVGXT::collect_wide_chars(const std::wstring &wide_text)
{
	for (auto wchar : wide_text)
	{
		if (wchar > 0xFF)
		{
			m_collection.insert(wchar);
		}
	}
}

