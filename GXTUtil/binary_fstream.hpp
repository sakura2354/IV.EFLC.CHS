#pragma once
#include <fstream>
#include <cstddef>
#include <type_traits>

class binary_fstream :public std::fstream
{
public:
	typedef std::fstream base_type;

	using base_type::read;
	using base_type::write;

	binary_fstream() = default;

	binary_fstream(const binary_fstream &) = delete;
	binary_fstream &operator=(const binary_fstream &) = delete;

	explicit binary_fstream::binary_fstream(const char* filename, std::ios::openmode mode)
		:base_type(filename, mode | std::ios::binary)
	{

	}

	explicit binary_fstream(const std::string &filename, std::ios::openmode mode = std::ios::in | std::ios::out)
		:base_type(filename, mode | std::ios::binary)
	{

	}

	binary_fstream(binary_fstream &&rv)
		:base_type(std::move(rv))
	{

	}

	template <typename T>
	std::enable_if_t<std::is_trivial<T>::value, binary_fstream &> read(T &result)
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
	std::enable_if_t<std::is_trivial<T>::value, binary_fstream &> write(const T &object)
	{
		write(reinterpret_cast<const char *>(&object), sizeof(T));
		return (*this);
	}

	template <typename container>
	std::enable_if_t<std::is_trivial<typename container::value_type>::value, binary_fstream &> write_container(const container &cont)
	{
		for (auto &element : cont)
		{
			write(element);
		}

		return (*this);
	}
};
