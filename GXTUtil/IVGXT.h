#pragma once

class binary_fstream;

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

struct TABL_ENTRY
{
	char m_name[8];
	std::int32_t m_offset;
};

struct TKEY_ENTRY
{
	std::int32_t m_offset;
	std::uint32_t m_hash;
};

struct GXT_HEADER
{
	std::int32_t m_magic;

	bool valid() const
	{
		return (m_magic == 0x100004);
	}
};

struct TABL_HEADER
{
	char m_tabl[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tabl[0] == 'T' && m_tabl[1] == 'A' && m_tabl[2] == 'B' && m_tabl[3] == 'L');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TABL_ENTRY));
	}
};

struct TKEY_HEADER
{
	char m_name[8];
	char m_tkey[4];
	std::int32_t m_size;

	bool valid(const char *name) const
	{
		return (m_tkey[0] == 'T' && m_tkey[1] == 'K' && m_tkey[2] == 'E' && m_tkey[3] == 'Y' && std::strncmp(m_name, name, 8) == 0);
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TKEY_ENTRY));
	}
};

struct TKEY_HEADER_MAIN
{
	char m_tkey[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tkey[0] == 'T' && m_tkey[1] == 'K' && m_tkey[2] == 'E' && m_tkey[3] == 'Y');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TKEY_ENTRY));
	}
};

struct TDAT_HEADER
{
	char m_tdat[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tdat[0] == 'T' && m_tdat[1] == 'D' && m_tdat[2] == 'A' && m_tdat[3] == 'T');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(wchar_t));
	}
};

struct TABLE_SORT
{
	bool operator()(const std::string &lhs, const std::string &rhs) const
	{
		if (rhs == "MAIN")
		{
			return false;
		}
		else if (lhs == "MAIN")
		{
			return true;
		}
		else
		{
			return (lhs < rhs);
		}
	}
};

class IVGXT
{
public:
	static bool has_ext_txt(const std::string &name);
	static bool has_ext_gxt(const std::string &name);

	void txt2gxt(const std::string &input_path, const std::string &output_path);
	void gxt2txt(const std::string &input_path);

	void load_text(const std::string &text_path);
	void load_binary(const std::string &binary_path);
	
	void generate_text();
	void generate_binary(const std::string &binary_path);
	void generate_collection(const std::string &collection_path);
	void generate_table(const std::string &table_path);

	void process(const std::string &input_path, const std::string &output_path);

private:
	std::map<std::string, std::vector<std::pair<std::uint32_t, std::pair<std::string, std::wstring>>>, TABLE_SORT> m_data;
	std::set<wchar_t> m_collection;

	static void transform_characters(std::vector<wchar_t> &input);
	static void transform_characters(std::wstring &input);

	static std::wstring to_wide(const std::string &input);
	static std::string to_narrow(const std::wstring &input);

	static void skip_utf8_signature(std::ifstream &text_stream);
	static void add_utf8_signature(std::ofstream &text_stream);

	void collect_wide_chars(const std::wstring &wide_text);

	static std::size_t get_data_size(const std::vector<std::pair<std::uint32_t, std::pair<std::string, std::wstring>>> &table);
};
