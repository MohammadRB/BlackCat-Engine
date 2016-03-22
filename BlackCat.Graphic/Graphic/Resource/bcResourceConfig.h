// [01/04/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/Resource/Buffer/bcBufferConfig.h"
#include "Graphic/Resource/Texture/bcTextureConfig.h"
#include "Graphic/Resource/View/bcResourceViewConfig.h"
#include "Graphic/Resource/View/bcRenderTargetViewConfig.h"
#include "Graphic/Resource/View/bcDepthStencilViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		class BC_GRAPHIC_DLL_EXP bc_buffer_configure_modifier
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

		class BC_GRAPHIC_DLL_EXP bc_texture_configure_modifier
		{
		public:
			explicit bc_texture_configure_modifier(bc_texture_config p_config) noexcept;

			bc_texture_config as_depth_stencil_texture() noexcept;

			bc_texture_config as_render_target_texture() noexcept;

		protected:

		private:
			bc_texture_config m_config;
		};

		class BC_GRAPHIC_DLL_EXP bc_buffer_view_configure_modifier
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

		class BC_GRAPHIC_DLL_EXP bc_buffer_view_configure
		{
		public:
			explicit bc_buffer_view_configure(bc_format p_format) noexcept;

			bc_buffer_view_configure_modifier as_shader_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept;

			bc_buffer_view_configure_modifier as_unordered_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept;

		protected:

		private:
			bc_format m_format;
		};

		class BC_GRAPHIC_DLL_EXP bc_texture_view_configure_modifier
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

		class BC_GRAPHIC_DLL_EXP bc_texture_view_configure
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

		class BC_GRAPHIC_DLL_EXP bc_resource_configure
		{
		public:
			bc_buffer_configure_modifier as_buffer(bcUINT p_num_element,
				bcUINT p_element_size,
				bc_format p_format,
				bc_resource_usage p_usage,
				bc_resource_view_type p_view_types,
				bool p_as_stream_output = false) noexcept;

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