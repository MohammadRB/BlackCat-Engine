 // [10/11/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"
#include "Platform/Script/bcScriptReference.h"

namespace black_cat
{
	namespace platform
	{
		template<bc_platform TPlatform, typename T>
		struct bc_platform_script_ref_pack
		{
		};

		/**
		 * \brief Store a strong reference to given script object
		 * \tparam T 
		 */
		template<bc_platform TPlatform, typename T>
		class bc_platform_script_ref
		{
		public:
			using type = T;
			using platform_pack = bc_platform_script_ref_pack<TPlatform, type>;

		public:
			bc_platform_script_ref();

			template
			<
				typename = typename std::enable_if
				<
					std::is_base_of<bc_platform_script_reference<TPlatform>, std::decay_t<type>>::value
				>::type
			>
			explicit bc_platform_script_ref(const type& p_object);

			bc_platform_script_ref(bc_platform_script_ref&&) noexcept;

			~bc_platform_script_ref();

			bc_platform_script_ref& operator=(bc_platform_script_ref&&) noexcept;

			bc_platform_script_ref& operator=(const type& p_object);

			type* operator->() noexcept;

			type& operator*();

			type& get();

			const type& get() const;

			void reset();

			void reset(const type& p_object);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		private:
			platform_pack m_pack;
		};

		template<typename T>
		using bc_script_ref = bc_platform_script_ref<g_current_platform, T>;
	}
}