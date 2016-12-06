#pragma once

//Merge american.gxt

static_assert(sizeof(wchar_t) == 2, "Damn I need size 2 wide char!");

class CBinaryStream;

#include <cstdint>
#include <array>
#include <string>
#include <map>
#include <set>
#include <utility>

class IVGXT
{
public:
	struct TABL
	{
		char name[8];
		std::int32_t offset;
	};

	struct TKEY
	{
		std::int32_t offset;
		std::uint32_t hash;
	};

	struct table_sort
	{
		bool operator()(const std::array<char, 8> &lhs, const std::array<char, 8> &rhs) const;
	};

	static bool has_ext_txt(const std::string &name);
	static bool has_ext_gxt(const std::string &name);

	void txt2gxt(const std::string &input_path, const std::string &output_path);
	void gxt2txt(const std::string &input_path, const std::string &output_path);

	void load_text(const std::string &text_path);
	void load_binary(const std::string &binary_path);

	void generate_text(const std::string &text_path);
	void generate_binary(const std::string &binary_path);
	void generate_collection(const std::string &collection_path);
	void generate_table(const std::string &table_path);

	void process(const std::string &input_path, const std::string &output_path);

private:
	std::map<std::array<char, 8>, std::map<std::uint32_t, std::pair<std::string, std::wstring> >, table_sort> m_data;
	std::set<wchar_t> m_collection;

	static void to_wide(const std::string &input, std::wstring &output);
	static void to_narrow(const std::wstring &input, std::string &output);

	static void read_c_wstring(CBinaryStream &binary_stream, std::wstring &output);

	static void skip_utf8_signature(std::ifstream &text_stream);
	static void add_utf8_signature(std::ofstream &text_stream);

	void collect_wide_chars(const std::wstring &wide_text);

	static std::size_t get_data_size(const std::map<std::uint32_t, std::pair<std::string, std::wstring> > &table);
};
