// [01/10/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcDeviceObject.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_shader_type : bcBYTE
		{
			vertex = 1,
			hull = 2,
			domain = 4,
			geometry = 8,
			pixel = 16,
			compute = 32
		};

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_compiled_shader_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_compiled_shader : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_compiled_shader_pack<TRenderApi>;

		public:
			bc_platform_compiled_shader();

			bc_platform_compiled_shader(bc_platform_compiled_shader&& p_other);

			~bc_platform_compiled_shader();

			bc_platform_compiled_shader& operator=(bc_platform_compiled_shader&& p_other);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_compiled_shader = bc_platform_compiled_shader< g_current_platform_render_api >;
		using bc_compiled_shader_ptr = bc_resource_ptr< bc_compiled_shader >;

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_ishader_pack
		{
			
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_ishader : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_ishader_pack<TRenderApi>;

		public:
			bc_platform_ishader();

			virtual ~bc_platform_ishader() = 0;

			virtual bc_shader_type get_type() const = 0;

		protected:
			bc_platform_ishader(bc_platform_ishader&& p_other);

			bc_platform_ishader& operator=(bc_platform_ishader&& p_other);

			platform_pack m_pack;

		private:
		};

		using bc_ishader = bc_platform_ishader< g_current_platform_render_api >;
	}
}