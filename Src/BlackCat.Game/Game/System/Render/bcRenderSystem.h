// [04/28/2016 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcArray.h"
#include "Core/Messaging/Event/bcEvent.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcObjectPool.h"
#include "Graphic/bcEvent.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/bcExport.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcStateConfigs.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/State/bcComputeState.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderSystemParameter.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/System/Render/bcFrameRenderer.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
		class bc_query_manager;
	}

	namespace game
	{
		class bci_render_task;
		class bc_file_system;
		class bc_physics_system;
		class bc_render_thread_manager;
		class bc_material_manager;
		class bc_animation_manager;
		class bc_scene;
		
		struct bc_render_system_parameter
		{
			bc_render_system_parameter(bcUINT32 p_device_backbuffer_width, 
				bcUINT32 p_device_backbuffer_height, 
				graphic::bc_format p_device_backbuffer_format, 
				graphic::bc_device_output p_render_output)
				: m_device_backbuffer_width(p_device_backbuffer_width),
				m_device_backbuffer_height(p_device_backbuffer_height),
				m_device_backbuffer_format(p_device_backbuffer_format),
				m_render_output(std::move(p_render_output))
			{
			}

			bcUINT32 m_device_backbuffer_width;
			bcUINT32 m_device_backbuffer_height;
			graphic::bc_format m_device_backbuffer_format;
			graphic::bc_device_output m_render_output;
		};

		struct bc_render_system_update_context
		{
			bc_render_system_update_context(const core_platform::bc_clock::update_param& p_clock,
				const bci_camera& p_camera)
				: m_clock(p_clock),
				m_camera(p_camera)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			const bci_camera& m_camera;
		};

		struct bc_render_system_render_context
		{
			bc_render_system_render_context(const core_platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager)
				: m_clock(p_clock),
				m_query_manager(p_query_manager)
			{
			}

			core_platform::bc_clock::update_param m_clock;
			core::bc_query_manager& m_query_manager;
		};

		struct bc_render_system_swap_context
		{
			bc_render_system_swap_context(const core_platform::bc_clock::update_param& p_clock)
				: m_clock(p_clock)
			{
			}

			core_platform::bc_clock::update_param m_clock;
		};

		class BC_GAME_DLL bc_render_system : public core::bc_initializable<core::bc_content_stream_manager&, bc_physics_system&, bc_render_system_parameter>
		{
		private:
			friend class _bc_render_pass_state_handle_deleter;
			friend class _bc_render_state_handle_deleter;
			friend class _bc_compute_state_handle_deleter;

		public:
			using update_context = bc_render_system_update_context;
			using render_context = bc_render_system_render_context;
			using swap_context = bc_render_system_swap_context;

		public:
			bc_render_system();

			bc_render_system(bc_render_system&&) noexcept;

			~bc_render_system();

			bc_render_system& operator=(bc_render_system&&) noexcept;

			graphic::bc_device& get_device() noexcept;
			
			const graphic::bc_device& get_device() const noexcept;

			bc_material_manager& get_material_manager() noexcept;
			
			const bc_material_manager& get_material_manager() const noexcept;

			bc_animation_manager& get_animation_manager() noexcept;
			
			const bc_animation_manager& get_animation_manager() const noexcept;

			bc_shape_drawer& get_shape_drawer() noexcept;
			
			const bc_shape_drawer& get_shape_drawer() const noexcept;

			const graphic::bc_constant_buffer_parameter& get_global_cbuffer() const;

			const graphic::bc_constant_buffer_parameter& get_per_object_cbuffer() const;
			
			const graphic::bc_constant_buffer_parameter& get_per_skinned_object_cbuffer() const;

			template<class TPass>
			TPass* get_render_pass();

			template<class TPass>
			void add_render_pass(TPass p_pass);

			template<class TPass, class TPassBefore>
			void add_render_pass_before(TPass p_pass);

			template<typename TPass>
			bool remove_render_pass();
			
			void update(const update_context& p_update_params);

			void render(const render_context& p_render_param);

			void swap(const swap_context& p_swap_context);

			/**
			 * \brief \b ThreadSafe
			 * \param p_task 
			 */
			void add_render_task(bci_render_task& p_task);

			/**
			 * \brief \b ThreadSafe
			 * \param p_vertex_shader_name 
			 * \param p_hull_shader_name 
			 * \param p_domain_shader_name 
			 * \param p_geometry_shader_name 
			 * \param p_pixel_shader_name 
			 * \param p_vertex_layout 
			 * \param p_blend 
			 * \param p_depth_stencil 
			 * \param p_rasterizer 
			 * \param p_sample_mask 
			 * \param p_render_target_formats 
			 * \param p_depth_stencil_format 
			 * \param p_ms_config 
			 * \return 
			 */
			graphic::bc_device_pipeline_state_ref create_device_pipeline_state(const bcCHAR* p_vertex_shader_name,
				const bcCHAR* p_hull_shader_name,
				const bcCHAR* p_domain_shader_name,
				const bcCHAR* p_geometry_shader_name,
				const bcCHAR* p_pixel_shader_name,
				bc_vertex_type p_vertex_layout,
				bc_blend_type p_blend,
				bc_depth_stencil_type p_depth_stencil,
				bc_rasterizer_type p_rasterizer,
				bcUINT p_sample_mask,
				std::initializer_list<bc_surface_format_type> p_render_target_formats,
				bc_surface_format_type p_depth_stencil_format,
				bc_multi_sample_type p_ms_config);

			/**
			 * \brief \n \b ThreadSafe
			 * \param p_compute_shader_name 
			 * \return 
			 */
			graphic::bc_device_compute_state_ref create_device_compute_state(const bcCHAR* p_compute_shader_name);

			/**
			 * \brief Shader parameter register indices will be re-indexed based on their ordering in the array.
			 * \n \b ThreadSafe
			 * \param p_pipeline_state 
			 * \param p_viewport 
			 * \param p_shader_targets 
			 * \param p_shader_depth 
			 * \param p_shader_samplers 
			 * \param p_resource_views
			 * \param p_unordered_views 
			 * \param p_shader_buffers 
			 * \return 
			 */
			bc_render_pass_state_ptr create_render_pass_state(graphic::bc_device_pipeline_state p_pipeline_state,
				graphic::bc_viewport p_viewport,
				bc_render_pass_state_render_target_view_array p_shader_targets,
				graphic::bc_depth_stencil_view p_shader_depth,
				bc_render_pass_state_sampler_array p_shader_samplers,
				bc_render_pass_state_resource_view_array p_resource_views,
				bc_render_pass_state_unordered_view_array p_unordered_views,
				bc_render_pass_state_constant_buffer_array p_shader_buffers);

			/**
			 * \brief Shader parameter register indices will be re-indexed based on their ordering in the array.
			 * \n \b ThreadSafe
			 * \param p_primitive 
			 * \param p_vertex_buffer
			 * \param p_vertex_buffer_stride 
			 * \param p_vertex_buffer_offset 
			 * \param p_index_buffer 
			 * \param p_index_type 
			 * \param p_index_count 
			 * \param p_index_buffer_offset 
			 * \param p_resource_views 
			 * \param p_shader_buffers 
			 * \return 
			 */
			bc_render_state_ptr create_render_state(graphic::bc_primitive p_primitive,
				graphic::bc_buffer p_vertex_buffer,
				bcUINT32 p_vertex_buffer_stride,
				bcUINT32 p_vertex_buffer_offset,
				graphic::bc_buffer p_index_buffer,
				bc_index_type p_index_type,
				bcUINT32 p_index_count,
				bcUINT32 p_index_buffer_offset,
				bc_render_state_resource_view_array p_resource_views,
				bc_render_state_constant_buffer_array p_shader_buffers);

			/**
			 * \brief Shader parameter register indices will be re-indexed based on their ordering in the array.
			 * \n \b ThreadSafe
			 * \param p_compute_state 
			 * \param p_samplers 
			 * \param p_resource_views 
			 * \param p_unordered_views 
			 * \param p_cbuffers 
			 * \return 
			 */
			bc_compute_state_ptr create_compute_state(graphic::bc_device_compute_state p_compute_state,
				bc_compute_state_sampler_array p_samplers,
				bc_compute_state_resource_view_array p_resource_views,
				bc_compute_state_unordered_view_array p_unordered_views,
				bc_compute_state_constant_buffer_array p_cbuffers);

			/**
			 * \brief Destroy all registered render passes to release probable references in queries
			 */
			void destroy_render_passes();
			
		private:			
			void _initialize(core::bc_content_stream_manager& p_content_stream, bc_physics_system& p_physics_system, bc_render_system_parameter p_parameter) override;

			void _destroy() override;

			bool _event_handler(core::bci_event& p_event);

			/**
			 * \brief ThreadSafe function
			 * \param p_render_pass_state
			 */
			void _destroy_render_pass_state(bc_render_pass_state* p_render_pass_state);

			/**
			 * \brief ThreadSafe function
			 * \param p_render_state
			 */
			void _destroy_render_state(bc_render_state* p_render_state);

			/**
			 * \brief ThreadSafe function
			 * \param p_compute_state
			 */
			void _destroy_compute_state(bc_compute_state* p_compute_state);

			graphic::bc_device m_device;
			core::bc_concurrent_object_pool<bc_render_pass_state> m_render_pass_states;
			core::bc_concurrent_object_pool<bc_render_state> m_render_states;
			core::bc_concurrent_object_pool<bc_compute_state> m_compute_states;
			
			core::bc_content_stream_manager* m_content_stream;
			core::bc_unique_ptr<bc_render_thread_manager> m_thread_manager;
			core::bc_unique_ptr<bc_material_manager> m_material_manager;
			core::bc_unique_ptr<bc_render_pass_manager> m_render_pass_manager;
			core::bc_unique_ptr<bc_animation_manager> m_animation_manager;
			core::bc_unique_ptr<bc_shape_drawer> m_shape_drawer;
			core::bc_unique_ptr<bc_frame_renderer> m_frame_renderer;

			core::bc_nullable<graphic::bc_app_event_device_reset> m_device_reset_event;
			core::bc_event_listener_handle m_window_resize_handle;
			core::bc_event_listener_handle m_app_active_handle;
			core::bc_event_listener_handle m_device_reset_handle;
			core::bc_event_listener_handle m_frame_render_finish_handle;
		};

		inline graphic::bc_device& bc_render_system::get_device() noexcept
		{
			return m_device;
		}

		inline const graphic::bc_device& bc_render_system::get_device() const noexcept
		{
			return m_device;
		}

		inline bc_material_manager& bc_render_system::get_material_manager() noexcept
		{
			return *m_material_manager;
		}

		inline const bc_material_manager& bc_render_system::get_material_manager() const noexcept
		{
			return *m_material_manager;
		}

		inline bc_animation_manager& bc_render_system::get_animation_manager() noexcept
		{
			return *m_animation_manager;
		}

		inline const bc_animation_manager& bc_render_system::get_animation_manager() const noexcept
		{
			return *m_animation_manager;
		}
				
		inline bc_shape_drawer& bc_render_system::get_shape_drawer() noexcept
		{
			return *m_shape_drawer;
		}

		inline const bc_shape_drawer& bc_render_system::get_shape_drawer() const noexcept
		{
			return *m_shape_drawer;
		}

		inline const graphic::bc_constant_buffer_parameter& bc_render_system::get_global_cbuffer() const
		{
			return m_frame_renderer->get_global_cbuffer();
		}

		inline const graphic::bc_constant_buffer_parameter& bc_render_system::get_per_object_cbuffer() const
		{
			return m_frame_renderer->get_per_object_cbuffer();
		}

		inline const graphic::bc_constant_buffer_parameter& bc_render_system::get_per_skinned_object_cbuffer() const
		{
			return m_frame_renderer->get_per_skinned_object_cbuffer();
		}
		
		template<typename TPass>
		TPass* bc_render_system::get_render_pass()
		{
			return m_render_pass_manager->get_pass<TPass>();
		}

		template<typename TPass>
		void bc_render_system::add_render_pass(TPass p_pass)
		{
			m_render_pass_manager->add_pass<TPass>(std::move(p_pass));

			auto l_pass = m_render_pass_manager->get_pass<TPass>();

			l_pass->initialize_resources(*this);
		}

		template<class TPass, class TPassBefore>
		void bc_render_system::add_render_pass_before(TPass p_pass)
		{
			m_render_pass_manager->add_pass_before<TPass, TPassBefore>(std::move(p_pass));

			auto l_pass = m_render_pass_manager->get_pass<TPass>();

			l_pass->initialize_resources(*this);
		}

		template<typename TPass>
		bool bc_render_system::remove_render_pass()
		{
			auto* l_pass = m_render_pass_manager->get_pass<TPass>();

			if (!l_pass)
			{
				return false;
			}

			l_pass->destroy(m_device);

			return m_render_pass_manager->remove_pass<TPass>();
		}
	}
}