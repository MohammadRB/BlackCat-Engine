// [06/12/2016 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcVector.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_instance
		{
		public:
			explicit bc_render_instance(const core::bc_matrix4f& p_world)
				: m_transform(p_world)
			{
			}

			bc_render_instance(const bc_render_instance&) = default;

			~bc_render_instance() = default;

			bc_render_instance& operator=(const bc_render_instance&) = default;

			const core::bc_matrix4f& get_transform() const
			{
				return m_transform;
			}

			void set_transform(const core::bc_matrix4f& p_world)
			{
				m_transform = p_world;
			}
			
		private:
			core::bc_matrix4f m_transform;
		};

		class bc_skinned_render_instance
		{
		public:
			explicit bc_skinned_render_instance(bcSIZE p_count)
			{
				m_transforms.resize(p_count);
			}
			
			bc_skinned_render_instance(const core::bc_matrix4f* p_transforms, bcSIZE p_count)
			{
				m_transforms.resize(p_count);
				set_transforms(p_transforms);
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

			void set_transforms(const core::bc_matrix4f* p_transforms)
			{
				std::memcpy(m_transforms.data(), p_transforms, m_transforms.size());
			}

		private:
			core::bc_vector_movable<core::bc_matrix4f> m_transforms;
		};
	}
}