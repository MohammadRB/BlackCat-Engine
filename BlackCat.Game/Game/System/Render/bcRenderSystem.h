// [04/28/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcArray.h"
#include "Core/Event/bcEvent.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderPassManager.h"
#include "Game/System/Render/bcStateConfigs.h"
#include "Game/System/Render/bcRenderState.h"
#include "Game/System/Render/bcRenderPassState.h"
#include "Game/System/Render/bcComputeState.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderSystemParameter.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/bcRenderPass.h"
#include "Game/Object/Scene/bcScenceGraph.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class bc_irender_task;
		class bc_file_system;
		class bc_render_thread_manager;
		class bc_material_manager;
		
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

		struct bc_render_system_update_param : public core_platform::bc_clock::update_param
		{
			bc_render_system_update_param(const core_platform::bc_clock::update_param& p_clock_update,
				const core::bc_vector3f p_camera_position,
				const core::bc_matrix4f& p_view,
				const core::bc_matrix4f& p_projection,
				const bc_icamera::extend& p_camera_extends)
				: update_param(p_clock_update),
				m_camera_position(p_camera_position),
				m_view_matrix(p_view),
				m_projection_matrix(p_projection),
				m_camera_extends(p_camera_extends)
			{
			}

			core::bc_vector3f m_camera_position;
			core::bc_matrix4f m_view_matrix;
			core::bc_matrix4f m_projection_matrix;
			bc_icamera::extend m_camera_extends;
		};

		class BC_GAME_DLL bc_render_system : public core::bc_initializable< core::bc_content_stream_manager&, bc_render_system_parameter >
		{
		private:
			friend class _bc_render_state_handle_deleter;
			friend class _bc_render_pass_state_handle_deleter;
			friend class _bc_compute_state_handle_deleter;
			using render_state_entry = std::pair< core::bc_nullable< bc_render_state >, core::bc_vector_frame< bc_render_instance > >;

		public:
			using update_param = bc_render_system_update_param;

		public:
			bc_render_system();

			bc_render_system(bc_render_system&&) = default;

			~bc_render_system();

			bc_render_system& operator=(bc_render_system&&) = default;

			graphic::bc_device& get_device()
			{
				return m_device;
			}

			bc_material_manager& get_material_manager() noexcept
			{
				return *m_material_manager.get();
			}

			/**
			 * \brief Thread safe function
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
			graphic::bc_device_pipeline_state_ptr create_device_pipeline_state(const bcCHAR* p_vertex_shader_name,
				const bcCHAR* p_hull_shader_name,
				const bcCHAR* p_domain_shader_name,
				const bcCHAR* p_geometry_shader_name,
				const bcCHAR* p_pixel_shader_name,
				bc_vertex_type p_vertex_layout,
				bc_blend_type p_blend,
				bc_depth_stencil_type p_depth_stencil,
				bc_rasterizer_type p_rasterizer,
				bcUINT p_sample_mask,
				std::initializer_list< bc_surface_format_type > p_render_target_formats,
				bc_surface_format_type p_depth_stencil_format,
				bc_multi_sample_type p_ms_config);

			/**
			 * \brief Thread safe function
			 * \param p_compute_shader_name 
			 * \return 
			 */
			graphic::bc_device_compute_state_ptr create_device_compute_state(const bcCHAR* p_compute_shader_name);

			/**
			 * \brief Shader parameter register indices will be reindexed based on their ordering in the array.
			 * Thread safe function
			 * \param p_pipeline_state 
			 * \param p_viewport 
			 * \param p_shader_targets 
			 * \param p_shader_depth 
			 * \param p_shader_samplers 
			 * \param p_resource_views 
			 * \param p_shader_buffers 
			 * \return 
			 */
			bc_render_pass_state_ptr create_render_pass_state(graphic::bc_device_pipeline_state p_pipeline_state,
				graphic::bc_viewport p_viewport,
				bc_render_pass_state_render_target_view_array&& p_shader_targets,
				graphic::bc_depth_stencil_view p_shader_depth,
				bc_render_pass_state_sampler_array&& p_shader_samplers,
				bc_render_pass_state_resource_view_array&& p_resource_views,
				bc_render_pass_state_constant_buffer_array&& p_shader_buffers);

			/**
			 * \brief Shader parameter register indices will be reindexed based on their ordering in the array.
			 * Thread safe function
			 * \param p_primitive 
			 * \param p_vertex_buffer 
			 * \param p_verext_buffer_offset 
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
				bcUINT32 p_verext_buffer_offset,
				graphic::bc_buffer p_index_buffer,
				bc_index_type p_index_type,
				bcUINT32 p_index_count,
				bcUINT32 p_index_buffer_offset,
				bc_render_state_resource_view_array&& p_resource_views,
				bc_render_state_constant_buffer_array&& p_shader_buffers);

			/**
			 * \brief Shader parameter register indices will be reindexed based on their ordering in the array.
			 * Thread safe function
			 * \param p_compute_state 
			 * \param p_dispatch_x 
			 * \param p_dispatch_y 
			 * \param p_dispatch_z 
			 * \param p_samplers 
			 * \param p_resource_views 
			 * \param p_unordered_views 
			 * \param p_cbuffers 
			 * \return 
			 */
			bc_compute_state_ptr create_compute_state(graphic::bc_device_compute_state p_compute_state,
				bcUINT32 p_dispatch_x,
				bcUINT32 p_dispatch_y,
				bcUINT32 p_dispatch_z,
				bc_compute_state_sampler_array&& p_samplers,
				bc_compute_state_resource_view_array&& p_resource_views,
				bc_compute_state_unordered_view_array&& p_unordered_views,
				bc_compute_state_constant_buffer_array&& p_cbuffers);

			/**
			 * \brief Thread safe function
			 * \param p_render_pass_state 
			 */
			void destroy_render_pass_state(bc_render_pass_state* p_render_pass_state);

			/**
			 * \brief Thread safe function
			 * \param p_render_state 
			 */
			void destroy_render_state(bc_render_state* p_render_state);

			/**
			 * \brief Thread safe function
			 * \param p_compute_state 
			 */
			void destroy_compute_state(bc_compute_state* p_compute_state);

			template< typename TPass >
			void add_render_pass(bcUINT p_location, core::bc_unique_ptr< TPass >&& p_pass);

			template< typename T >
			T* get_render_pass();

			bool remove_render_pass(bcUINT p_location);

			template< typename TPass >
			bool remove_render_pass();

			/**
			 * \brief Add a render instance to render queue
			 * \param p_state 
			 * \param p_instance 
			 */
			void add_render_instance(const bc_render_state* p_state, const bc_render_instance& p_instance);

			/**
			 * \brief Render all instances in render queue
			 */
			void render_all_instances(bc_render_thread& p_render_thread);

			/**
			 * \brief Clear render queue. After rendering instances this function must be called
			 */
			void clear_render_instances();

			void update(const update_param& p_update_params);

			void render(bc_scene& p_scene);

			void add_render_task(bc_irender_task& p_task);

		protected:

		private:
			void _initialize(core::bc_content_stream_manager& p_content_stream, bc_render_system_parameter p_parameter) override;

			void _destroy() override;

			bool _event_handler(core::bc_ievent& p_event);

			core::bc_content_stream_manager* m_content_stream;
			graphic::bc_device m_device;
			core::bc_unique_ptr< bc_render_thread_manager > m_thread_manager;
			core::bc_unique_ptr< bc_material_manager > m_material_manager;

			core::bc_event_listener_handle m_window_resize_handle;
			core::bc_event_listener_handle m_device_listener_handle;

			bc_render_pass_manager m_render_pass_manager;

			core_platform::bc_mutex m_render_states_mutex;
			core::bc_vector_movale< core::bc_nullable< bc_render_pass_state > > m_render_pass_states;
			core::bc_vector_movale< render_state_entry > m_render_states;
			core::bc_vector_movale< core::bc_nullable< bc_compute_state > > m_compute_states;

			update_param m_last_update_params;
			graphic::bc_constant_buffer_parameter m_global_cbuffer_parameter;
			graphic::bc_constant_buffer_parameter m_perobject_cbuffer_parameter;
		};

		template< typename TPass >
		void bc_render_system::add_render_pass(bcUINT p_location, core::bc_unique_ptr<TPass>&& p_pass)
		{
			m_render_pass_manager.add_pass(p_location, std::move(p_pass));

			auto l_pass = m_render_pass_manager.get_pass(p_location);

			l_pass->initialize_resources(*this);
		}

		template< typename T >
		T* bc_render_system::get_render_pass()
		{
			return m_render_pass_manager.get_pass< T >();
		}

		template< typename TPass >
		bool bc_render_system::remove_render_pass()
		{
			auto* l_pass = m_render_pass_manager.get_pass<TPass>();

			if (!l_pass)
			{
				return false;
			}

			l_pass->destroy(m_device);

			return m_render_pass_manager.remove_pass<TPass>();
		}
	}
}