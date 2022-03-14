// [03/12/2022 MRB]

#pragma once

#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcSoundChannel.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_sound_component : public bci_actor_component
		{
			BC_COMPONENT(sound, true, false)

		public:
			bc_sound_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_sound_component(bc_sound_component&& p_other) noexcept = default;

			~bc_sound_component() noexcept override = default;

			bc_sound_component& operator=(bc_sound_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			sound::bc_sound get_sound() const noexcept;

			sound::bc_sound_channel get_channel() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

		private:
			bool m_auto_play;
			sound::bc_sound m_sound;
			sound::bc_sound_channel m_channel;
		};

		inline sound::bc_sound bc_sound_component::get_sound() const noexcept
		{
			return m_sound;
		}

		inline sound::bc_sound_channel bc_sound_component::get_channel() const noexcept
		{
			return m_channel;
		}
	}
}
