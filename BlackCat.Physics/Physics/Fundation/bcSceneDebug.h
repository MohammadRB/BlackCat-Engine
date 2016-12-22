// [12/06/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "Physics/bcPhysicsApi.h"

namespace black_cat
{
	namespace physics
	{
		template<bc_physics_api TApi>
		class bc_platform_scene;

		struct bc_scene_debug_line
		{
			bc_scene_debug_line(const core::bc_vector3f& p_point0,
				bcUINT32 p_color0,
				const core::bc_vector3f& p_point1,
				bcUINT32 p_color1)
				: m_point0(p_point0),
				m_color0(p_color0),
				m_point1(p_point1),
				m_color1(p_color1)
			{
			}

			const core::bc_vector3f m_point0;
			const bcUINT32 m_color0;
			const core::bc_vector3f m_point1;
			const bcUINT32 m_color1;
		};

		struct bc_scene_debug_text
		{
			bc_scene_debug_text(const core::bc_vector3f& p_position, bcUINT32 p_color, bcUINT32 p_size, const bcCHAR* p_text)
				: m_position(p_position),
				m_color(p_color),
				m_size(p_size),
				m_text(p_text)
			{
			}

			const core::bc_vector3f m_position;
			const bcUINT32 m_color;
			const bcUINT32 m_size;
			const bcCHAR* m_text;
		};

		template< bc_physics_api TApi >
		struct bc_platform_scene_debug_pack
		{
		};

		template<bc_physics_api TApi>
		class bc_platform_scene_debug
		{
		public:
			using platform_pack = bc_platform_scene_debug_pack< TApi >;
			friend class bc_platform_scene<TApi>;

		public:
			bc_platform_scene_debug(bc_platform_scene_debug&&) noexcept;

			~bc_platform_scene_debug();

			bc_platform_scene_debug& operator=(bc_platform_scene_debug&&) noexcept;
			
			bcUINT32 get_line_count() const noexcept;

			core::bc_vector< bc_scene_debug_line > get_lines() const noexcept;

			bcUINT32 get_text_count() const noexcept;

			core::bc_vector< bc_scene_debug_text > get_texts() const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_platform_scene_debug() noexcept;

			platform_pack m_pack;
		};

		using bc_scene_debug = bc_platform_scene_debug< g_current_physics_api >;
	}
}