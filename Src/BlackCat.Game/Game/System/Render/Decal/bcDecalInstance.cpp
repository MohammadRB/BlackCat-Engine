// [23/02/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalManagerContainer.h"

namespace black_cat::game
{
	void bc_decal_instance_deleter::operator()(bc_decal_instance* p_ptr) const
	{
		m_container->destroy_decal_instance(p_ptr);
	}

	void bc_decal_instance::set_local_position(const core::bc_vector3f& p_position) noexcept
	{
		auto l_local_transform = core::bc_matrix4f::identity();
		l_local_transform.set_rotation(get_local_rotation());
		l_local_transform.set_translation(get_local_position());

		auto l_world_transform = m_world_transform;
		l_world_transform.make_neutralize_scale();
		l_world_transform = l_local_transform.inverse() * l_world_transform;

		m_local_position = p_position;
		set_world_transform(l_world_transform);
	}

	void bc_decal_instance::set_local_rotation(const core::bc_matrix3f& p_rotation) noexcept
	{
		auto l_local_transform = core::bc_matrix4f::identity();
		l_local_transform.set_rotation(get_local_rotation());
		l_local_transform.set_translation(get_local_position());

		auto l_world_transform = m_world_transform;
		l_world_transform.make_neutralize_scale();
		l_world_transform = l_local_transform.inverse() * l_world_transform;

		m_local_rotation = p_rotation;
		set_world_transform(l_world_transform);
	}
		
	void bc_decal_instance::set_world_transform(const core::bc_matrix4f& p_world_transform) noexcept
	{
		auto l_local_transform = core::bc_matrix4f::identity();
		l_local_transform.set_rotation(get_local_rotation());
		l_local_transform.set_translation(get_local_position());

		const auto l_decal_width = m_decal->get_width();
		const auto l_decal_height = m_decal->get_height();
		const auto l_decal_depth = m_decal->get_depth();
		const auto l_scale = core::bc_matrix4f::scale_matrix_xyz(l_decal_width, l_decal_depth, l_decal_height);
			
		m_world_transform = l_local_transform * p_world_transform;
		auto l_world_rotation = m_world_transform.get_rotation();
		l_world_rotation.make_neutralize_scale();
		m_world_transform.set_rotation(l_scale.get_rotation() * l_world_rotation);
	}

	std::pair<physics::bc_shape_box, physics::bc_transform> bc_decal_instance::get_box() const noexcept
	{
		return std::make_pair
		(
			physics::bc_shape_box(m_decal->get_width() / 2, m_decal->get_depth() / 2, m_decal->get_height() / 2),
			physics::bc_transform(get_world_transform())
		);
	}
}
