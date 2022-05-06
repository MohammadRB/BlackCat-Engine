#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		void bc_string_to_lower(bc_string& p_str)
		{
			std::transform
			(
				std::begin(p_str),
				std::end(p_str),
				std::begin(p_str),
				[](bcCHAR c)
				{
					return std::tolower(c);
				}
			);
		}

		void bc_string_to_lower(bc_wstring& p_str)
		{
			std::transform
			(
				std::begin(p_str),
				std::end(p_str),
				std::begin(p_str),
				[](bcWCHAR c)
				{
					return std::tolower(c);
				}
			);
		}

		void bc_string_to_upper(bc_string& p_str)
		{
			std::transform
			(
				std::begin(p_str),
				std::end(p_str),
				std::begin(p_str),
				[](bcCHAR c)
				{
					return std::toupper(c);
				}
			);
		}

		void bc_string_to_upper(bc_wstring& p_str)
		{
			std::transform
			(
				std::begin(p_str),
				std::end(p_str),
				std::begin(p_str),
				[](bcWCHAR c)
				{
					return std::toupper(c);
				}
			);
		}

		bool bc_string_begins_with(bc_string_view p_str, bc_string_view p_search_str)
		{
			if (p_str.size() < p_search_str.size())
			{
				return false;
			}

			return std::equal
			(
				std::begin(p_search_str),
				std::next(std::begin(p_search_str), p_search_str.size()),
				std::begin(p_str),
				[](bcCHAR ch1, bcCHAR ch2)
				{
					return std::tolower(ch1) == std::tolower(ch2);
				}
			);
		}

		bool bc_string_begins_with(bc_wstring_view p_str, bc_wstring_view p_search_str)
		{
			if (p_str.size() < p_search_str.size())
			{
				return false;
			}

			return std::equal
			(
				std::begin(p_search_str),
				std::next(std::begin(p_search_str), p_search_str.size()),
				std::begin(p_str),
				[](bcWCHAR ch1, bcWCHAR ch2)
				{
					return std::tolower(ch1) == std::tolower(ch2);
				}
			);
		}

		bool bc_string_ends_with(bc_string_view p_str, bc_string_view p_search_str)
		{
			if (p_str.size() < p_search_str.size())
			{
				return false;
			}

			return std::equal
			(
				std::rbegin(p_search_str),
				std::next(std::rbegin(p_search_str), p_search_str.size()),
				std::rbegin(p_str),
				[](bcCHAR ch1, bcCHAR ch2)
				{
					return std::tolower(ch1) == std::tolower(ch2);
				}
			);
		}

		bool bc_string_ends_with(bc_wstring_view p_str, bc_wstring_view p_search_str)
		{
			if (p_str.size() < p_search_str.size())
			{
				return false;
			}

			return std::equal
			(
				std::rbegin(p_search_str),
				std::next(std::rbegin(p_search_str), p_search_str.size()),
				std::rbegin(p_str),
				[](bcWCHAR ch1, bcWCHAR ch2)
				{
					return std::tolower(ch1) == std::tolower(ch2);
				}
			);
		}
	}
}