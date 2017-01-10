// [01/04/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/Resource/Buffer/bcBufferConfig.h"
#include "Graphic/Resource/Texture/bcTextureConfig.h"
#include "Graphic/Resource/View/bcResourceViewConfig.h"
#include "Graphic/Resource/View/bcRenderTargetViewConfig.h"
#include "Graphic/Resource/View/bcDepthStencilViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		class BC_GRAPHIC_DLL bc_buffer_configure_modifier
		{
		public:
			explicit bc_buffer_configure_modifier(bc_buffer_config p_config) noexcept;

			bc_buffer_config as_vertex_buffer() noexcept;

			bc_buffer_config as_index_buffer() noexcept;

			bc_buffer_config as_constant_buffer() noexcept;

			bc_buffer_config as_structured_buffer(bcUINT p_element_size) noexcept;

			bc_buffer_config as_append_consume(bcUINT p_element_size) noexcept;

			bc_buffer_config as_byte_address_buffer() noexcept;

			bc_buffer_config as_indirect_args_buffer() noexcept;

		protected:

		private:
			bc_buffer_config m_config;
		};

		class BC_GRAPHIC_DLL bc_texture_configure_modifier
		{
		public:
			explicit bc_texture_configure_modifier(bc_texture_config p_config) noexcept;

			bc_texture_config as_normal_texture() noexcept;

			bc_texture_config as_render_target_texture() noexcept;

			bc_texture_config as_depth_stencil_texture() noexcept;

		protected:

		private:
			bc_texture_config m_config;
		};

		class BC_GRAPHIC_DLL bc_buffer_view_configure_modifier
		{
		public:
			explicit bc_buffer_view_configure_modifier(bc_resource_view_config p_config) noexcept;

			bc_resource_view_config as_structured_buffer() noexcept;

			bc_resource_view_config as_append_consume() noexcept;

			bc_resource_view_config as_byte_address_buffer() noexcept;

			bc_resource_view_config as_indirect_args_buffer() noexcept;

		protected:

		private:
			bc_resource_view_config m_config;
		};

		class BC_GRAPHIC_DLL bc_buffer_view_configure
		{
		public:
			explicit bc_buffer_view_configure(bc_format p_format) noexcept;

			bc_buffer_view_configure_modifier as_shader_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept;

			bc_buffer_view_configure_modifier as_unordered_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept;

		protected:

		private:
			bc_format m_format;
		};

		class BC_GRAPHIC_DLL bc_texture_view_configure_modifier
		{
		public:
			friend class bc_texture_view_configure;

		public:
			bc_texture_view_configure_modifier(bc_resource_view_config p_config) noexcept;

			bc_resource_view_config on_texture2d() noexcept;

			bc_resource_view_config on_depth_stencil_texture2d() noexcept;

			bc_resource_view_config on_render_target_texture2d() noexcept;

		protected:

		private:
			bc_resource_view_config m_config;
		};

		class BC_GRAPHIC_DLL bc_texture_view_configure
		{
		public:
			explicit bc_texture_view_configure(bc_format p_format) noexcept;

			bc_texture_view_configure_modifier as_tex2d_shader_view(bcUINT p_most_detailed_mip, bcUINT p_mip_levels) noexcept;

			bc_texture_view_configure_modifier as_tex2dms_shader_view() noexcept;

			bc_texture_view_configure_modifier as_tex2d_array_shader_view(bcUINT p_most_detailed_mip,
				bcUINT p_mip_levels,
				bcUINT p_first_array_slice,
				bcUINT p_array_size) noexcept;

			bc_texture_view_configure_modifier as_tex2dms_array_shader_view(bcUINT p_first_array_slice, bcUINT p_array_size) noexcept;

			bc_texture_view_configure_modifier as_tex2d_unordered_shader_view(bcUINT p_mip_slice) noexcept;

			bc_texture_view_configure_modifier as_tex2d_array_unordered_shader_view(bcUINT p_mip_slice,
				bcUINT p_first_array_slice,
				bcUINT p_array_size) noexcept;

			bc_resource_view_config as_texcube_shader_view(bcUINT p_most_detailed_mip, bcUINT p_mip_levels) noexcept;

			bc_render_target_view_config as_tex2d_render_target_view(bcUINT p_mip_slice) noexcept;

			bc_resource_view_config as_tex2d_array_render_target_view(bcUINT p_mip_slice, bcUINT p_first_array_slice, bcUINT p_array_size) noexcept;

			bc_render_target_view_config as_tex2dms_render_target_view() noexcept;

			bc_resource_view_config as_tex2dms_array_render_target_view(bcUINT p_first_array_slice, bcUINT p_array_size) noexcept;

			bc_depth_stencil_view_config as_tex2d_depth_stencil_view(bcUINT p_mip_slice) noexcept;

			bc_resource_view_config as_tex2d_array_depth_stencil_view(bcUINT p_mip_slice, bcUINT p_first_array_slice, bcUINT p_array_size) noexcept;

			bc_depth_stencil_view_config as_tex2dms_depth_stencil_view() noexcept;

			bc_resource_view_config as_tex2dms_array_depth_stencil_view(bcUINT p_first_array_slice, bcUINT p_array_size) noexcept;

		protected:

		private:
			bc_format m_format;
		};
		
		class bc_resource_view_configure
		{
		public:
			bc_buffer_view_configure as_buffer_view(bc_format p_format) noexcept
			{
				return bc_buffer_view_configure(p_format);
			}

			bc_texture_view_configure as_texture_view(bc_format p_format) noexcept
			{
				return bc_texture_view_configure(p_format);
			}
		};

		class BC_GRAPHIC_DLL bc_resource_configure
		{
		public:
			bc_buffer_configure_modifier as_buffer(bcUINT p_num_element,
				bcUINT p_element_size,
				bc_resource_usage p_usage,
				bc_resource_view_type p_view_types = bc_resource_view_type::none,
				bool p_as_stream_output = false) noexcept;

			/**
			 * \brief 
			 * \param p_width Width of texture to generate
			 * \param p_height Height of texture to generate
			 * \param p_mip_generation True if p_mip_levels is 0 or greater that 1
			 * \param p_mip_levels 0 for auto mip level generation or a value greater than 1 to enable mip generation
			 * \param p_format Format of texture to generate
			 * \param p_usage Resource access usage by CPU and GPU
			 * \param p_view_types The views that will be created on this texture
			 * \param p_sample_count Number of samples in multisampling textures
			 * \param p_sample_quality Quality of samples in multisampling textures
			 * \param p_array_size Size of array in it is a texture array
			 * \return 
			 */
			bc_texture_configure_modifier as_texture2d(bcUINT p_width,
				bcUINT p_height,
				bool p_mip_generation,
				bcUINT p_mip_levels,
				bc_format p_format,
				bc_resource_usage p_usage,
				bc_resource_view_type p_view_types,
				bcUINT p_sample_count = 1,
				bcUINT p_sample_quality = 0,
				bcUINT p_array_size = 1) noexcept;
		};

		class bc_graphic_resource_configure
		{
		public:
			bc_resource_configure as_resource() noexcept
			{
				return bc_resource_configure();
			}

			bc_resource_view_configure as_resource_view() noexcept
			{
				return bc_resource_view_configure();
			}
		};
	}
}