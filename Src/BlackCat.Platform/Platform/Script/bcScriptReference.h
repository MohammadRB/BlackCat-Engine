// [02/10/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace platform
	{
		template<bc_platform TPlatform>
		struct bc_platform_script_reference_pack
		{
		};

		/**
		 * \brief This class provide an abstraction over api's reference values so that bc_script_ref through
		 * this class can create strong reference to api values
		 */
		template<bc_platform TPlatform>
		class bc_platform_script_reference
		{
		public:
			using platform_pack = bc_platform_script_reference_pack<TPlatform>;

		public:
			bc_platform_script_reference(const bc_platform_script_reference&) noexcept;

			virtual ~bc_platform_script_reference();

			bc_platform_script_reference& operator=(const bc_platform_script_reference&) noexcept;

			virtual bool is_valid() const noexcept = 0;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:
			bc_platform_script_reference();

		private:
			platform_pack m_pack;
		};

		using bc_script_reference = bc_platform_script_reference<g_current_platform>;
	}
}