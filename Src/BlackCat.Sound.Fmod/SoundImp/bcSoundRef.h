// [03/09/2022 MRB]

#pragma once

#include "SoundImp/bcSoundReference.h"

#include "Sound/bcSoundRef.h"
#include "SoundImp/bcExport.h"

namespace FMOD
{
	class Sound;
}

namespace black_cat
{
	namespace sound
	{
		template<bc_sound_api>
		class bc_platform_sound;

		template<bc_sound_api>
		class bc_platform_channel_group;

		template<class TObject>
		struct bc_platform_sounds_ref_pack<bc_sound_api::fmod, TObject>
		{
			TObject m_object;
		};

		template<bc_sound_api TApi, class TObject>
		bc_platform_sounds_ref<TApi, TObject>::bc_platform_sounds_ref() noexcept = default;

		template<bc_sound_api TApi, class TObject>
		template<typename>
		bc_platform_sounds_ref<TApi, TObject>::bc_platform_sounds_ref(const type& p_object) noexcept
			: m_pack{p_object}
		{
		}

		template<bc_sound_api TApi, class TObject>
		bc_platform_sounds_ref<TApi, TObject>::bc_platform_sounds_ref(bc_platform_sounds_ref&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<bc_sound_api TApi, class TObject>
		bc_platform_sounds_ref<TApi, TObject>::~bc_platform_sounds_ref()
		{
			reset();
		}

		template<bc_sound_api TApi, class TObject>
		bc_platform_sounds_ref<TApi, TObject>& bc_platform_sounds_ref<TApi, TObject>::operator=(bc_platform_sounds_ref&& p_other) noexcept
		{
			reset(p_other.m_pack.m_object);
			p_other.m_pack.m_object = type();

			return *this;
		}

		template<bc_sound_api TApi, class TObject>
		typename bc_platform_sounds_ref<TApi, TObject>::type* bc_platform_sounds_ref<TApi, TObject>::operator->() noexcept
		{
			return &get();
		}

		template<bc_sound_api TApi, class TObject>
		const typename bc_platform_sounds_ref<TApi, TObject>::type* bc_platform_sounds_ref<TApi, TObject>::operator->() const noexcept
		{
			return &get();
		}

		template<bc_sound_api TApi, class TObject>
		typename bc_platform_sounds_ref<TApi, TObject>::type& bc_platform_sounds_ref<TApi, TObject>::operator*() noexcept
		{
			return get();
		}

		template<bc_sound_api TApi, class TObject>
		const typename bc_platform_sounds_ref<TApi, TObject>::type& bc_platform_sounds_ref<TApi, TObject>::operator*() const noexcept
		{
			return get();
		}

		template<bc_sound_api TApi, class TObject>
		typename bc_platform_sounds_ref<TApi, TObject>::type& bc_platform_sounds_ref<TApi, TObject>::get() noexcept
		{
			return m_pack.m_object;
		}

		template<bc_sound_api TApi, class TObject>
		const typename bc_platform_sounds_ref<TApi, TObject>::type& bc_platform_sounds_ref<TApi, TObject>::get() const noexcept
		{
			return m_pack.m_object;
		}

		template<bc_sound_api TApi, class TObject>
		typename bc_platform_sounds_ref<TApi, TObject>::type bc_platform_sounds_ref<TApi, TObject>::release() noexcept
		{
			auto l_object = m_pack.m_object;
			m_pack.m_object = TObject();

			return l_object;
		}

		template<bc_sound_api TApi, class TObject>
		void bc_platform_sounds_ref<TApi, TObject>::reset()
		{
			auto& l_reference = static_cast<bc_platform_sound_reference<TApi>&>(m_pack.m_object);
			if (!l_reference.is_valid())
			{
				return;
			}

			_release_fmod_reference(m_pack.m_object);
			m_pack.m_object = type();
		}

		template<bc_sound_api TApi, class TObject>
		void bc_platform_sounds_ref<TApi, TObject>::reset(const type& p_object)
		{
			reset();
			m_pack.m_object = p_object;
		}

		void BC_SOUNDIMP_DLL _release_fmod_reference(bc_platform_sound<bc_sound_api::fmod>& p_reference);

		void BC_SOUNDIMP_DLL _release_fmod_reference(bc_platform_channel_group<bc_sound_api::fmod>& p_reference);
	}
}
