// [03/11/2022 MRB]

#pragma once

#include "Core/Concurrency/bcTask.h"
#include "SoundImp/Fundation/bcDevice.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_sound_manager;

		struct bc_sound_system_params
		{
			bcUINT32 m_max_channels;
		};

		struct bc_sound_system_update_context
		{
			bc_sound_system_update_context(const platform::bc_clock::update_param& p_clock, const bc_camera_instance& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			platform::bc_clock::update_param m_clock;
			const bc_camera_instance& m_camera;
		};

		class BC_GAME_DLL bc_sound_system : public core::bc_initializable<bc_sound_system_params>
		{
		public:
			using update_context = bc_sound_system_update_context;

		public:
			bc_sound_system() noexcept = default;

			bc_sound_system(bc_sound_system&& p_other) noexcept = default;

			~bc_sound_system() noexcept = default;

			bc_sound_system& operator=(bc_sound_system&& p_other) noexcept = default;

			sound::bc_device& get_device() noexcept;

			const sound::bc_device& get_device() const noexcept;

			bc_sound_manager create_sound_manager() noexcept;

			void update(const update_context& p_context);

			core::bc_task<void> update_async(const update_context& p_context);

		private:
			void _initialize(bc_sound_system_params) override;

			void _destroy() override;

			sound::bc_device m_device;
			core::bc_vector3f m_last_listener_position;
		};

		inline sound::bc_device& bc_sound_system::get_device() noexcept
		{
			return m_device;
		}

		inline const sound::bc_device& bc_sound_system::get_device() const noexcept
		{
			return m_device;
		}
	}
}
