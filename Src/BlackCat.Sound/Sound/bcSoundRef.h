// [03/09/2022 MRB]

#pragma once

#include "Sound/bcSoundReference.h"
#include "Sound/bcSoundApi.h"

namespace black_cat
{
	namespace sound
	{
		template<bc_sound_api TApi, class TObject>
		struct bc_platform_sounds_ref_pack
		{
		};

		/**
		 * \brief Store a strong reference to sound reference objects
		 * \tparam TApi
		 * \tparam TObject
		 */
		template<bc_sound_api TApi, class TObject>
		class bc_platform_sounds_ref
		{
		public:
			using type = TObject;
			using platform_pack = bc_platform_sounds_ref_pack<TApi, TObject>;

		public:
			bc_platform_sounds_ref() noexcept;

			template
			<
				typename = std::enable_if_t
				<
					std::is_base_of_v<bc_platform_sound_reference<TApi>, std::decay_t<type>>
				>
			>
			explicit bc_platform_sounds_ref(const type& p_object) noexcept;

			bc_platform_sounds_ref(bc_platform_sounds_ref&& p_other) noexcept;

			~bc_platform_sounds_ref();

			bc_platform_sounds_ref& operator=(bc_platform_sounds_ref&& p_other) noexcept;

			type* operator->() noexcept;

			const type* operator->() const noexcept;

			type& operator*() noexcept;

			const type& operator*() const noexcept;

			type& get() noexcept;

			const type& get() const noexcept;

			type release() noexcept;

			void reset();

			void reset(const type& p_object);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		template<class TObject>
		using bc_sounds_ref = bc_platform_sounds_ref<g_current_sound_api, TObject>;
	}
}
