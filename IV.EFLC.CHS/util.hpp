#pragma once
#include <cstdint>
#include <utility>
#include "deps/xliveless/xliveless.h"

template <typename T, std::uintptr_t address>
struct mem_reader
{
	typedef T element_type;
	typedef T *pointer;
	typedef T &reference;

	static pointer get_pointer()
	{
		return reinterpret_cast<pointer>(address + dwLoadOffset);
	}

	static reference get_reference()
	{
		return *get_pointer();
	}

	static element_type get_value()
	{
		return get_reference();
	}
};

template <std::uintptr_t address, typename ret, typename... arg_types>
struct c_func_caller
{
	static ret call(arg_types... args)
	{
		return ((ret(__cdecl *)(arg_types...))(address + dwLoadOffset))(std::forward<arg_types>(args)...);
	}
};

template <std::uintptr_t address, typename ret, typename... arg_types>
struct std_func_caller
{
	static ret call(arg_types... args)
	{
		return ((ret(__stdcall *)(arg_types...))(address + dwLoadOffset))(std::forward<arg_types>(args)...);
	}
};

template <std::uintptr_t address, typename ret, typename... arg_types>
struct this_func_caller
{
	static ret call(void *this_pointer, arg_types... args)
	{
		return ((ret(__thiscall *)(void *, arg_types...))(address + dwLoadOffset))(this_pointer, std::forward<arg_types>(args)...);
	}
};

template <std::uintptr_t address, typename ret, typename... arg_types>
struct fast_func_caller
{
	static ret call(arg_types... args)
	{
		return ((ret(__fastcall *)(arg_types...))(address + dwLoadOffset))(std::forward<arg_types>(args)...);
	}
};
