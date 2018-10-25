// [01/10/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"

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

		template< bc_render_api TRenderApi >
		struct bc_platform_compiled_shader_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_compiled_shader : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_compiled_shader_pack<TRenderApi>;

		public:
			bc_platform_compiled_shader();

			explicit bc_platform_compiled_shader(platform_pack& p_pack);

			bc_platform_compiled_shader(const bc_platform_compiled_shader& p_other);

			~bc_platform_compiled_shader();

			bc_platform_compiled_shader& operator=(const bc_platform_compiled_shader& p_other);

			bool is_valid() const noexcept override;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_compiled_shader = bc_platform_compiled_shader< g_current_render_api >;
		using bc_compiled_shader_ptr = bc_device_ref< bc_compiled_shader >;

		template< bc_render_api TRenderApi >
		struct bc_platform_ishader_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_ishader : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_ishader_pack<TRenderApi>;

		public:
			virtual ~bc_platform_ishader();

			virtual bc_shader_type get_type() const = 0;

		protected:
			bc_platform_ishader();

			explicit bc_platform_ishader(platform_pack& p_pack);

			bc_platform_ishader(const bc_platform_ishader& p_other);

			bc_platform_ishader& operator=(const bc_platform_ishader& p_other);

			platform_pack m_pack;

		private:
		};

		using bc_ishader = bc_platform_ishader< g_current_render_api >;
	}
}