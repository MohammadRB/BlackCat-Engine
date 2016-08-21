// [03/23/2016 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		class bc_enum
		{
		public:
			template< typename TEnum >
			static TEnum none()
			{
				using type = typename std::underlying_type<TEnum>::type;

				return static_cast<TEnum>(type(0));
			}

			template< typename TEnum >
			static TEnum or(std::initializer_list<TEnum> p_values)
			{
				using type = typename std::underlying_type<TEnum>::type;

				return std::accumulate(std::begin(p_values), std::end(p_values), static_cast< TEnum >(0), [](TEnum p_first, TEnum p_second)
				{
					return static_cast< TEnum >(static_cast<type>(p_first) | static_cast<type>(p_second));
				});
			}

			template< typename TEnum >
			static TEnum and(std::initializer_list<TEnum> p_values)
			{
				using type = typename std::underlying_type<TEnum>::type;
				const type l_max = (std::numeric_limits<type>::max)();

				return std::accumulate(std::begin(p_values), std::end(p_values), static_cast< TEnum >(l_max), [](TEnum p_first, TEnum p_second)
				{
					return static_cast< TEnum >(static_cast<type>(p_first) & static_cast<type>(p_second));
				});
			}

			template< typename TEnum >
			static bool has(TEnum p_values, TEnum p_value)
			{
				return bc_enum::and({ p_values, p_value }) == p_value;
			}

			static constexpr bcUINT32 value(const bcCHAR* p_str) // Bug
			{
				return bc_compile_time_string_hash(p_str);
			}
		};
	}
}
