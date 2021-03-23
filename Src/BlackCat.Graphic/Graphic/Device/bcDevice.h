// [12/31/2015 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Container/bcAllocator.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/Device/bcDeviceInfo.h"
#include "Graphic/Device/bcDeviceOutput.h"
#include "Graphic/Resource/bcResource.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_buffer;
		using bc_buffer = bc_platform_buffer< g_current_render_api >;
		using bc_buffer_ref = bc_device_ref< bc_buffer >;

		template<bc_render_api>
		class bc_platform_buffer_config;
		using bc_buffer_config = bc_platform_buffer_config< g_current_render_api >;

		template<bc_render_api>
		class bc_platform_texture_config;
		using bc_texture_config = bc_platform_texture_config< g_current_render_api >;

		class bc_subresource_data;

		template<bc_render_api>
		class bc_platform_texture2d;
		using bc_texture2d = bc_platform_texture2d< g_current_render_api >;
		using bc_texture2d_ref = bc_device_ref< bc_texture2d >;

		struct bc_sampler_state_config;

		template<bc_render_api>
		class bc_platform_sampler_state;
		using bc_sampler_state = bc_platform_sampler_state< g_current_render_api >;
		using bc_sampler_state_ref = bc_device_ref< bc_sampler_state >;

		struct bc_shader_macro;
		
		template<bc_render_api>
		class bc_platform_compiled_shader;
		using bc_compiled_shader = bc_platform_compiled_shader< g_current_render_api >;
		using bc_compiled_shader_ptr = bc_device_ref< bc_compiled_shader >;

		template<bc_render_api>
		class bc_platform_vertex_shader;
		using bc_vertex_shader = bc_platform_vertex_shader< g_current_render_api >;
		using bc_vertex_shader_ref = bc_device_ref<bc_vertex_shader>;

		template<bc_render_api>
		class bc_platform_hull_shader;
		using bc_hull_shader = bc_platform_hull_shader< g_current_render_api >;
		using bc_hull_shader_ref = bc_device_ref< bc_hull_shader >;

		template<bc_render_api>
		class bc_platform_domain_shader;
		using bc_domain_shader = bc_platform_domain_shader< g_current_render_api >;
		using bc_domain_shader_ref = bc_device_ref< bc_domain_shader >;

		template<bc_render_api>
		class bc_platform_geometry_shader;
		using bc_geometry_shader = bc_platform_geometry_shader< g_current_render_api >;
		using bc_geometry_shader_ref = bc_device_ref< bc_geometry_shader >;

		template<bc_render_api>
		class bc_platform_pixel_shader;
		using bc_pixel_shader = bc_platform_pixel_shader< g_current_render_api >;
		using bc_pixel_shader_ref = bc_device_ref< bc_pixel_shader >;

		template<bc_render_api>
		class bc_platform_compute_shader;
		using bc_compute_shader = bc_platform_compute_shader< g_current_render_api >;
		using bc_compute_shader_ref = bc_device_ref< bc_compute_shader >;

		template<bc_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_render_api >;
		using bc_device_pipeline_ref = bc_device_ref< bc_device_pipeline >;

		struct bc_device_pipeline_state_config;

		template<bc_render_api>
		class bc_platform_device_pipeline_state;
		using bc_device_pipeline_state = bc_platform_device_pipeline_state< g_current_render_api >;
		using bc_device_pipeline_state_ref = bc_device_ref<bc_device_pipeline_state>;

		struct bc_device_compute_state_config;

		template<bc_render_api>
		class bc_platform_device_compute_state;
		using bc_device_compute_state = bc_platform_device_compute_state< g_current_render_api >;
		using bc_device_compute_state_ref = bc_device_ref<bc_device_compute_state>;

		template<bc_render_api>
		class bc_platform_device_command_list;
		using bc_device_command_list = bc_platform_device_command_list< g_current_render_api >;
		using bc_device_command_list_ref = bc_device_ref< bc_device_command_list >;

		template<bc_render_api>
		class bc_platform_device_command_executor;
		using bc_device_command_executor = bc_platform_device_command_executor< g_current_render_api >;
		using bc_device_command_executor_ref = bc_device_ref< bc_device_command_executor >;

		template<bc_render_api>
		class bc_platform_resource_view_config;
		using bc_resource_view_config = bc_platform_resource_view_config< g_current_render_api >;

		template<bc_render_api>
		class bc_platform_render_target_view_config;
		using bc_render_target_view_config = bc_platform_render_target_view_config< g_current_render_api >;

		template<bc_render_api>
		class bc_platform_depth_stencil_view_config;
		using bc_depth_stencil_view_config = bc_platform_depth_stencil_view_config< g_current_render_api >;

		template<bc_render_api>
		class bci_platform_resource_view;
		using bci_resource_view = bci_platform_resource_view< g_current_render_api >;

		template<bc_render_api>
		class bc_platform_resource_view;
		using bc_resource_view = bc_platform_resource_view< g_current_render_api >;
		using bc_resource_view_ref = bc_device_ref< bc_resource_view >;

		template<bc_render_api>
		class bc_platform_depth_stencil_view;
		using bc_depth_stencil_view = bc_platform_depth_stencil_view< g_current_render_api >;
		using bc_depth_stencil_view_ref = bc_device_ref< bc_depth_stencil_view >;

		template<bc_render_api>
		class bc_platform_render_target_view;
		using bc_render_target_view = bc_platform_render_target_view< g_current_render_api >;
		using bc_render_target_view_ref = bc_device_ref<bc_render_target_view>;

		template< bc_render_api TRenderApi >
		class bc_platform_device_text_renderer;
		using bc_device_text_renderer = bc_platform_device_text_renderer< g_current_render_api >;
		
		template< bc_render_api TRenderApi >
		struct bc_platform_device_pack
		{
		};

		/**
		 * \brief ThreadSafe 
		 * \tparam TRenderApi 
		 */
		template< bc_render_api TRenderApi >
		class bc_platform_device 
			: public core::bc_initializable<bcUINT, bcUINT, bc_format, bc_device_output>,
			public core::bc_object_allocator,
			public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_device_pack<TRenderApi>;

		public:
			bc_platform_device();

			bc_platform_device(bc_platform_device&& p_other) noexcept;

			~bc_platform_device();

			bc_platform_device& operator=(bc_platform_device&& p_other) noexcept;

			bool get_vsync() const;

			void set_vsync(bool p_sync);

			bool get_full_screen() const;

			void set_full_screen(bool p_full_screen);

			bcUINT32 get_back_buffer_width() const;

			bcUINT32 get_back_buffer_height() const;

			bc_format get_back_buffer_format() const;

			bc_texture2d get_back_buffer_texture() const;

			bc_device_info get_device_info() const;

			bcUINT check_multi_sampling(bc_format p_textue_format, bcUINT p_sample_count) const;

			bc_buffer_ref create_buffer(bc_buffer_config& p_config, bc_subresource_data* p_data);

			bc_texture2d_ref create_texture2d(bc_texture_config& p_config, bc_subresource_data* p_data);

			bc_texture2d_ref create_texture2d(bc_texture_config& p_config, const bcBYTE* p_data, bcSIZE p_data_size, bc_image_format p_format);

			void save_texture2d(bc_texture2d p_texture, bc_image_format p_format, const bcECHAR* p_path);

			bc_sampler_state_ref create_sampler_state(const bc_sampler_state_config& p_config);

			bc_compiled_shader_ptr compile_vertex_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_vertex_shader_ref create_vertex_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_compiled_shader_ptr compile_hull_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_hull_shader_ref create_hull_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_compiled_shader_ptr compile_domain_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_domain_shader_ref create_domain_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_compiled_shader_ptr compile_geometry_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_geometry_shader_ref create_geometry_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_compiled_shader_ptr compile_pixel_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_pixel_shader_ref create_pixel_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_compiled_shader_ptr compile_compute_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file, const bc_shader_macro* p_macros = nullptr, bcUINT32 p_macro_count = 0);

			bc_compute_shader_ref create_compute_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function);

			bc_resource_view_ref create_resource_view(bci_resource& p_resource, bc_resource_view_config& p_view_config);

			bc_depth_stencil_view_ref create_depth_stencil_view(bci_resource& p_resource, bc_depth_stencil_view_config& p_view_config);

			bc_render_target_view_ref create_render_target_view(bci_resource& p_resource, bc_render_target_view_config& p_view_config);

			bc_device_pipeline_state_ref create_pipeline_state(bc_device_pipeline_state_config& p_config);

			bc_device_compute_state_ref create_compute_state(bc_device_compute_state_config& p_config);

			bc_device_pipeline_ref get_default_pipeline();
			
			bc_device_pipeline_ref create_pipeline();
			
			bc_device_command_list_ref create_command_list();

			bc_device_command_executor_ref create_command_executor();

			bc_device_text_renderer create_text_renderer();

			bc_mapped_resource map_resource(bci_resource& p_resource, bcUINT p_subresource, bc_resource_map p_map_type);

			void unmap_resource(bci_resource& p_resource, bcUINT p_subresource);

			/**
			 * \brief Resize back buffer and send bc_app_event_device_reset event to resize all other resizable resources. 
			 * If device is in fullscreen mode change resolution too.
			 * \param p_width 
			 * \param p_height 
			 */
			void resize_back_buffer(bcUINT p_width, bcUINT p_height);

			/**
			* \brief Resize back buffer and send bc_app_event_device_reset event to resize all other resizable resources.
			* If device is in fullscreen mode change resolution too.
			* \param p_width
			* \param p_height
			* \param p_format
			*/
			void resize_back_buffer(bcUINT p_width, bcUINT p_height, bc_format p_format);

			/**
			 * \brief Resize texture and recreate all resource views on that texture that are passed to the function.
			 * Before calling this function all strong references to the texture and resource views must be released.
			 * \param p_texture 
			 * \param p_width 
			 * \param p_height 
			 * \param p_views 
			 * \param p_num_views 
			 */
			void resize_texture2d(bc_texture2d& p_texture, 
				bcUINT p_width, 
				bcUINT p_height, 
				bci_resource_view** p_views,
				bcUINT p_num_views);

			void present();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			void _initialize(bcUINT p_width, bcUINT p_height, bc_format p_back_buffer_format, bc_device_output p_output) override;

			void _destroy() override;

			platform_pack m_pack;
		};

		using bc_device = bc_platform_device< g_current_render_api >;

		template< bc_render_api TRenderApi >
		bcUINT32 bc_platform_device<TRenderApi>::get_back_buffer_width() const
		{
			return get_back_buffer_texture().get_width();
		}

		template< bc_render_api TRenderApi >
		bcUINT32 bc_platform_device<TRenderApi>::get_back_buffer_height() const
		{
			return get_back_buffer_texture().get_height();
		}

		template< bc_render_api TRenderApi >
		bc_format bc_platform_device<TRenderApi>::get_back_buffer_format() const
		{
			return get_back_buffer_texture().get_format();
		}
	}
}