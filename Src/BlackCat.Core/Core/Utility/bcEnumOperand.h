// [23/03/2016 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat::core
{
	class bc_enum
	{
	public:
		template<typename TEnum>
		using enum_t = std::underlying_type_t<TEnum>;

		static constexpr bcINT value(bcINT p_index)
		{
			return 1 << p_index;
		}

		template< typename TEnum >
		static TEnum none()
		{
			using type = enum_t<TEnum>;

			return static_cast<TEnum>(type(0));
		}

		template<typename TEnum>
		static TEnum all()
		{
			using type = enum_t<TEnum>;

			return static_cast<TEnum>(type(std::numeric_limits<type>::max()));
		}

		template< typename TEnum >
		static TEnum mask_or(std::initializer_list<TEnum> p_values)
		{
			using type = enum_t<TEnum>;

			return std::accumulate(std::begin(p_values), std::end(p_values), static_cast< TEnum >(0), [](TEnum p_first, TEnum p_second)
			{
				return static_cast< TEnum >(static_cast<type>(p_first) | static_cast<type>(p_second));
			});
		}

		template< typename TEnum >
		static TEnum mask_and(std::initializer_list<TEnum> p_values)
		{
			using type = enum_t<TEnum>;
			const type l_max = (std::numeric_limits<type>::max)();

			return std::accumulate(std::begin(p_values), std::end(p_values), static_cast< TEnum >(l_max), [](TEnum p_first, TEnum p_second)
			{
				return static_cast< TEnum >(static_cast<type>(p_first) & static_cast<type>(p_second));
			});
		}

		template< typename TEnum >
		static TEnum set(TEnum p_values, TEnum p_value, bool p_bool)
		{
			using type = enum_t<TEnum>;

			if(p_bool)
			{
				return bc_enum::mask_or({ p_values, p_value });
			}

			return bc_enum::mask_and({ p_values, static_cast< TEnum >(~static_cast< type >(p_value)) });
		}

		template< typename TEnum >
		static bool has(TEnum p_values, TEnum p_value)
		{
			return bc_enum::mask_and({ p_values, p_value }) == p_value;
		}
	};
}
