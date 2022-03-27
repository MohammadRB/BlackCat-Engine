// [02/28/2020 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "GraphicImp/Device/bcDevice.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/System/Render/Light/bcDirectLight.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
	}
	
	namespace game
	{
		class bci_camera;
		class bc_render_system;
		class bc_render_thread_manager;
		class bc_render_pass_manager;
		class bc_render_state_buffer;

		struct bc_frame_renderer_update_context
		{
			bc_frame_renderer_update_context(const platform::bc_clock::update_param& p_clock, const bc_camera_instance& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			const platform::bc_clock::update_param& m_clock;
			const bc_camera_instance& m_camera;
		};

		struct bc_frame_renderer_render_context
		{
			bc_frame_renderer_render_context(const platform::bc_clock::update_param& p_clock, core::bc_query_manager& p_query_manager, bc_render_system& p_render_system)
				: m_clock(p_clock),
				m_query_manager(p_query_manager),
				m_render_system(p_render_system)
			{
			}

			const platform::bc_clock::update_param& m_clock;
			core::bc_query_manager& m_query_manager;
			bc_render_system& m_render_system;
		};
		
		class BC_GAME_DLL bc_frame_renderer
		{
		public:
			using update_context = bc_frame_renderer_update_context;
			using render_context = bc_frame_renderer_render_context;
			
		public:
			bc_frame_renderer(graphic::bc_device& p_device, bc_render_thread_manager& p_thread_manager, bc_render_pass_manager& p_render_pass_manager) noexcept;

			bc_frame_renderer(bc_frame_renderer&&) noexcept;

			~bc_frame_renderer();

			bc_frame_renderer& operator=(bc_frame_renderer&&) noexcept;

			static constexpr bool need_matrix_transpose() noexcept;
			
			const graphic::bc_constant_buffer_parameter& get_global_cbuffer() const noexcept;

			const graphic::bc_constant_buffer_parameter& get_per_object_cbuffer() const noexcept;
			
			const graphic::bc_constant_buffer_parameter& get_per_skinned_object_cbuffer() const noexcept;

			bc_render_thread_manager& get_thread_manager() noexcept;

			void update_global_cbuffer(bc_render_thread& p_render_thread,
				const platform::bc_clock::update_param& p_clock,
				const bc_camera_instance& p_camera);
			
			void update_global_cbuffer(bc_render_thread& p_render_thread, 
				const platform::bc_clock::update_param& p_clock, 
				const bc_camera_instance& p_camera,
				const bc_direct_light& p_global_light,
				const bc_direct_wind& p_global_wind);

			void update_per_object_cbuffer(bc_render_thread& p_render_thread, const bc_camera_instance& p_camera, const bc_render_instance& p_instance);
			
			void update_per_skinned_object_cbuffer(bc_render_thread& p_render_thread, const bc_camera_instance& p_camera, const bc_skinned_render_instance& p_instance);
			
			bc_render_state_buffer create_buffer() const;

			void render_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera);
			
			void render_skinned_buffer(bc_render_thread& p_render_thread, const bc_render_state_buffer& p_buffer, const bc_camera_instance& p_camera);
			
			void update(const update_context& p_update_param);

			void render(const render_context& p_render_param);

		private:
			bc_render_thread_manager* m_thread_manager;
			bc_render_pass_manager* m_render_pass_manager;

			graphic::bc_buffer_ref m_global_cbuffer;
			graphic::bc_buffer_ref m_per_object_cbuffer;
			graphic::bc_constant_buffer_parameter m_global_cbuffer_parameter;
			graphic::bc_constant_buffer_parameter m_per_object_cbuffer_parameter;

			core::bc_nullable<bc_camera_instance> m_prev_camera_instance;
			core::bc_nullable<bc_camera_instance> m_camera_instance;
			platform::bc_atomic<bc_camera_instance*> m_prev_camera;
			platform::bc_atomic<bc_camera_instance*> m_camera;
		};

		constexpr bool bc_frame_renderer::need_matrix_transpose() noexcept
		{
			return graphic::bc_render_api_info::use_column_matrix() && !core::bc_matrix4f::use_column_major_storage();
		}
		
		inline const graphic::bc_constant_buffer_parameter& bc_frame_renderer::get_global_cbuffer() const noexcept
		{
			return m_global_cbuffer_parameter;
		}

		inline const graphic::bc_constant_buffer_parameter& bc_frame_renderer::get_per_object_cbuffer() const noexcept
		{
			return m_per_object_cbuffer_parameter;
		}

		inline const graphic::bc_constant_buffer_parameter& bc_frame_renderer::get_per_skinned_object_cbuffer() const noexcept
		{
			return m_per_object_cbuffer_parameter;
		}

		inline bc_render_thread_manager& bc_frame_renderer::get_thread_manager() noexcept
		{
			return *m_thread_manager;
		}
	}
}