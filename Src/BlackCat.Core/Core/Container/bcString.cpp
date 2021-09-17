#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		bool bc_string_ends_with(const bcCHAR* p_str, bcSIZE p_str_len, const bcCHAR* p_search_str, bcSIZE p_search_str_len)
		{
			if (p_str_len >= p_search_str_len)
			{
				return std::equal
				(
					std::make_reverse_iterator(p_search_str),
					std::next(std::make_reverse_iterator(p_search_str), p_search_str_len),
					std::make_reverse_iterator(p_str),
					[](bcCHAR ch1, bcCHAR ch2)
					{
						return std::tolower(ch1) == std::tolower(ch2);
					}
				);
			}
			else
			{
				return false;
			}
		}

		bool bc_string_ends_with(const bcWCHAR* p_str, bcSIZE p_str_len, const bcWCHAR* p_search_str, bcSIZE p_search_str_len)
		{
			if (p_str_len >= p_search_str_len)
			{
				return std::equal
				(
					std::make_reverse_iterator(std::next(p_search_str, p_search_str_len)),
					std::make_reverse_iterator(p_search_str),
					std::make_reverse_iterator(std::next(p_str, p_str_len)),
					[](bcCHAR ch1, bcCHAR ch2)
					{
						return std::tolower(ch1) == std::tolower(ch2);
					}
				);
			}
			else
			{
				return false;
			}
		}
	}
}