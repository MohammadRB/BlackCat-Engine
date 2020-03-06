// [06/12/2016 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief This object represent part of device pipeline state configs that must be set for every instance
		 */
		class bc_render_instance
		{
		public:
			explicit bc_render_instance(const core::bc_matrix4f& p_world)
				: m_world(p_world)
			{
			}

			bc_render_instance(const bc_render_instance&) = default;

			~bc_render_instance() = default;

			bc_render_instance& operator=(const bc_render_instance&) = default;

			const core::bc_matrix4f& get_world() const
			{
				return m_world;
			}

			void set_world(const core::bc_matrix4f& p_world)
			{
				m_world = p_world;
			}
			
		private:
			core::bc_matrix4f m_world;
		};
	}
}