#pragma once
#include <iostream>
#include <utility>

template <typename first, typename ...rest>
void print_to_cout(first &&first_arg, rest &&...rest_args)
{
	std::cout << std::forward<first>(first_arg);
	print_to_cout(std::forward<rest>(rest_args)...);
}

template <typename last>
void print_to_cout(last &&last_arg)
{
	std::cout << std::forward<last>(last_arg) << std::endl;
}
