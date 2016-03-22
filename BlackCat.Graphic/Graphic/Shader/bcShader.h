// [01/10/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcDeviceObject.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_shader_type
		{
			vertex = 0,
			hull = 1,
			domain = 2,
			geometry = 3,
			pixel = 4,
			compute = 5
		};

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