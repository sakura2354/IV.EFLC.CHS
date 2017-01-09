#pragma once
#include <fstream>
#include <cstddef>
#include <type_traits>

class CBinaryStream :public std::fstream
{
public:
	typedef std::fstream base_type;

	using base_type::read;
	using base_type::write;

	CBinaryStream() = default;

	CBinaryStream(const CBinaryStream &) = delete;
	CBinaryStream &operator=(const CBinaryStream &) = delete;

	explicit CBinaryStream::CBinaryStream(const char* filename, std::ios_base::openmode mode)
		:base_type(filename, mode)
	{

	}

	explicit CBinaryStream(const std::string& filename, std::ios_base::openmode mode = ios_base::in | ios_base::out)
		:base_type(filename, mode)
	{

	}

	CBinaryStream(CBinaryStream &&rv)
		:base_type(std::move(rv))
	{

	}

	template <typename T>
	std::enable_if_t<std::is_trivial<T>::value, CBinaryStream &> read(T &result)
	{
		read(reinterpret_cast<char *>(&result), sizeof(T));
		return (*this);
	}

	template <typename T>
	std::enable_if_t<std::is_trivial<T>::value, T> read()
	{
		T result;

		read(result);

		return result;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial<T>::value, CBinaryStream &> write(const T &object)
	{
		write(reinterpret_cast<const char *>(&object), sizeof(T));
		return (*this);
	}

	template <typename container>
	std::enable_if_t<std::is_trivial<typename container::value_type>::value, CBinaryStream &> write_container(const container &cont)
	{
		for (auto &element : cont)
		{
			write(element);
		}

		return (*this);
	}
};
