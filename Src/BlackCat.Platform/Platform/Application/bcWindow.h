 // [23/10/2016 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace platform
	{
		template<bc_platform>
		struct bc_platform_window_pack
		{
		};

		template<bc_platform TPlatform>
		class bc_platform_window : private bc_no_copy
		{
		public:
			using platform_pack = bc_platform_window_pack<TPlatform>;
			using id = bcSIZE;
			constexpr static id invalid_id = static_cast<id>(0);

		public:
			bc_platform_window();

			bc_platform_window(bc_platform_window&& p_other) noexcept;

			virtual ~bc_platform_window();

			bc_platform_window& operator=(bc_platform_window&& p_other) noexcept;

			virtual id get_id() const = 0;

			virtual core::bc_estring get_caption() const = 0;

			virtual void set_caption(const bcECHAR* p_caption) = 0;
			
			/**
			 * \brief Update method for things like message dispatching.
			 * Implementation must propagate application events in response to OS events.
			 */
			virtual void update() = 0;

			/**
			 * \brief Before window being destructed close function must be called
			 */
			virtual void close() = 0;

			virtual platform_pack& get_platform_pack() = 0;

			virtual const platform_pack& get_platform_pack() const = 0;
		};

		using bc_window = bc_platform_window<g_current_platform>;
	}
}