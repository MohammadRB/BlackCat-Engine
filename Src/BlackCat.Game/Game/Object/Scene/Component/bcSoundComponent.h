// [03/12/2022 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Utility/bcNullable.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcChannel.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;

		struct _bc_sound_component_sound
		{
			core::bc_string m_name;
			sound::bc_sound_mode m_mode;
			core::bc_nullable<core::bc_vector2f> m_min_max_distance;
			core::bc_nullable<bcFLOAT> m_volume;
			bool m_auto_play;

			sound::bc_sound_content_ptr m_sound;
			sound::bc_channel m_channel;
		};

		class BC_GAME_DLL bc_sound_component : public bci_actor_component
		{
			BC_COMPONENT(sound, true, false)

		public:
			bc_sound_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_sound_component(bc_sound_component&& p_other) noexcept = default;

			~bc_sound_component() noexcept override = default;

			bc_sound_component& operator=(bc_sound_component&& p_other) noexcept = default;

			bc_actor get_actor() const noexcept override;

			/**
			 * \brief Get max sound length among all contained sounds
			 * \return 
			 */
			bcUINT32 get_max_length() const noexcept;

			sound::bc_sound get_sound(core::bc_string_view p_name) const noexcept;

			sound::bc_channel play_sound(core::bc_string_view p_name) const noexcept;

			/**
			 * \brief Play all contained sounds
			 */
			void play_sounds() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

		private:
			_bc_sound_component_sound* _find_sound(core::bc_string_view p_name) const;

			bc_scene* m_scene;
			core::bc_vector<_bc_sound_component_sound> m_sounds;
		};
	}
}
