// [02/28/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "GraphicImp/Device/bcDevice.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_icamera;
		class bc_render_system;
		class bc_render_thread_manager;
		class bc_render_pass_manager;
		class bc_render_state_buffer;

		struct bc_frame_renderer_update_param
		{
			bc_frame_renderer_update_param(const core_platform::bc_clock::update_param& p_clock, const bc_camera_instance& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			bc_camera_instance m_camera;
		};

		struct bc_frame_renderer_render_param
		{
			bc_frame_renderer_render_param(const core_platform::bc_clock::update_param& p_clock, bc_render_system& p_render_system)
				: m_clock(p_clock),
				m_render_system(p_render_system)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			bc_render_system& m_render_system;
		};
		
		class BC_GAME_DLL bc_frame_renderer
		{
		public:
			bc_frame_renderer(graphic::bc_device& p_device, bc_render_thread_manager& p_thread_manager, bc_render_pass_manager& p_render_pass_manager) noexcept;

			bc_frame_renderer(bc_frame_renderer&&) noexcept;

			~bc_frame_renderer();

			bc_frame_renderer& operator=(bc_frame_renderer&&) noexcept;

			const graphic::bc_constant_buffer_parameter& get_global_cbuffer() const;

			const graphic::bc_constant_buffer_parameter& get_per_object_cbuffer() const;

			void update_global_cbuffer(bc_render_thread& p_render_thread, const core_platform::bc_clock::update_param& p_clock, const bc_camera_instance& p_camera);
			
			bc_render_state_buffer create_buffer() const;

			void render_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera);
			
			void update(const bc_frame_renderer_update_param& p_update_param);

			void render(const bc_frame_renderer_render_param& p_render_param);

		private:
			bc_render_thread_manager* m_thread_manager;
			bc_render_pass_manager* m_render_pass_manager;

			graphic::bc_buffer_ptr m_global_cbuffer;
			graphic::bc_buffer_ptr m_per_object_cbuffer;
			graphic::bc_constant_buffer_parameter m_global_cbuffer_parameter;
			graphic::bc_constant_buffer_parameter m_per_object_cbuffer_parameter;

			core::bc_nullable<bc_camera_instance> m_prev_camera_instance;
			core::bc_nullable<bc_camera_instance> m_camera_instance;
			core_platform::bc_atomic<bc_camera_instance*> m_prev_camera;
			core_platform::bc_atomic<bc_camera_instance*> m_camera;
		};
	}
}