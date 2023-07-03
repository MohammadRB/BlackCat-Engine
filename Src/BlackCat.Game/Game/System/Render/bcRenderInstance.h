// [12/06/2016 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcEnumOperand.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_actor_render_group : bcUBYTE
		{
			unknown = 0,
			terrain = core::bc_enum::value(0),
			static_mesh = core::bc_enum::value(1),
			dynamic_mesh = core::bc_enum::value(2),
			vegetable = core::bc_enum::value(3),
			all_static = terrain | static_mesh,
			all_dynamic = dynamic_mesh,
			all = std::numeric_limits<bcUBYTE>::max()
		};
		
		class bc_render_instance
		{
		public:
			bc_render_instance(const core::bc_matrix4f& p_world, bc_actor_render_group p_group)
				: m_transform(p_world),
				m_group(p_group)
			{
			}

			bc_render_instance(const bc_render_instance&) = default;

			~bc_render_instance() = default;

			bc_render_instance& operator=(const bc_render_instance&) = default;

			const core::bc_matrix4f& get_transform() const
			{
				return m_transform;
			}

			bc_actor_render_group get_render_group() const noexcept
			{
				return m_group;
			}
		
		private:
			core::bc_matrix4f m_transform;
			bc_actor_render_group m_group;
		};

		class bc_skinned_render_instance
		{
		public:
			bc_skinned_render_instance(const core::bc_matrix4f* p_transforms, bcSIZE p_count, bc_actor_render_group p_group)
				: m_transforms(p_count),
				m_group(p_group)
			{
				std::memcpy(m_transforms.data(), p_transforms, p_count * sizeof(core::bc_matrix4f));
			}

			bc_skinned_render_instance(bc_skinned_render_instance&&) = default;

			~bc_skinned_render_instance() = default;

			bc_skinned_render_instance& operator=(bc_skinned_render_instance&&) = default;

			bcSIZE get_num_transforms() const noexcept
			{
				return m_transforms.size();
			}
			
			core::bc_matrix4f* get_transforms()
			{
				return m_transforms.data();
			}
			
			const core::bc_matrix4f* get_transforms() const
			{
				return m_transforms.data();
			}

			bc_actor_render_group get_render_group() const noexcept
			{
				return m_group;
			}
		
		private:
			core::bc_vector_movable<core::bc_matrix4f> m_transforms;
			bc_actor_render_group m_group;
		};
	}
}