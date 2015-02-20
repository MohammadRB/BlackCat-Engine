// [10/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"

namespace black_cat
{
	namespace core_platform
	{
		/*template< typename TB, typename T >
		struct bc_template_is
		{
		public:
			bc_template_is()
			{
				bcStaticAssert(std::is_base_of< TB, T >::value, "Invalid template argument");
			}

			~bc_template_is() {}

			static bool assert()
			{
				bcStaticAssert(std::is_base_of< TB, T >::value, "Invalid template argument");
			}
		};*/

		class bc_no_copy
		{
		public:
			bc_no_copy(const bc_no_copy&) = delete;

			bc_no_copy& operator=(const bc_no_copy&) = delete;

		protected:		
			bc_no_copy() = default;
			
			~bc_no_copy() = default;

		private:
		};

		class bc_no_copy_move : public bc_no_copy
		{
		public:
			bc_no_copy_move(const bc_no_copy_move&) = delete;

			bc_no_copy_move& operator=(const bc_no_copy_move&) = delete;

		protected:
			bc_no_copy_move() = default;

			~bc_no_copy_move() = default;

		private:
		};

		class bc_math_utility
		{
		public:
			static bcINT32 roundup(bcINT32 p_num, bcINT32 p_multipe)
			{
				return ((p_num + p_multipe - 1) / p_multipe) * p_multipe;
			}

			static bcINT32 roundup_p2(bcINT32 p_num, bcINT32 p_multipe)
			{
				return (p_num + p_multipe - 1) & ~(p_multipe - 1);
			}
		};
	}
}