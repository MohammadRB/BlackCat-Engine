// [02/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalManager.h"

namespace black_cat
{
	namespace game
	{
		void bc_decal_instance_deleter::operator()(bc_decal_instance* p_ptr) const
		{
			m_manager->destroy_decal_instance(p_ptr);
		}

		void bc_decal_instance::set_world_transform(const core::bc_matrix4f& p_transform) noexcept
		{
			core::bc_matrix4f l_decal_local_transform;
			l_decal_local_transform.make_identity();

			l_decal_local_transform.set_rotation(get_local_rotation());
			l_decal_local_transform.set_translation(get_local_position());

			const auto l_decal_width = m_decal->get_width();
			const auto l_decal_height = m_decal->get_height();
			const auto l_decal_depth = m_decal->get_depth();
			const auto l_scale = core::bc_matrix4f::scale_matrix_xyz(l_decal_width, l_decal_depth, l_decal_height);

			m_world_transform = l_scale * p_transform * l_decal_local_transform;
		}
	}	
}