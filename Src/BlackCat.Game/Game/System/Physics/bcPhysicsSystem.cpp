// [21/12/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Physics/bcActorGroup.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"

namespace black_cat::game
{
	bc_physics_system::bc_physics_system() = default;

	bc_physics_system::bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move) = default;

	bc_physics_system::~bc_physics_system() = default;

	bc_physics_system& bc_physics_system::operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move) = default;

	core::bc_unique_ptr<bcINT16> bc_physics_system::convert_height_field_samples(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples)
	{
		const bcUINT32 l_num_samples = p_num_row * p_num_column;
		core::bc_unique_ptr<bcINT16> l_buffer(static_cast<bcINT16*>(BC_ALLOC(sizeof(bcINT16) * l_num_samples, core::bc_alloc_type::frame)));
		bcINT16* l_buffer_ptr = l_buffer.get();

		for (bcUINT32 l_index = 0; l_index < l_num_samples; ++l_index)
		{
			l_buffer_ptr[l_index] = height_to_int16(p_samples[l_index]);
		}

		return l_buffer;
	}

	void bc_physics_system::create_px_shapes_from_height_map(const bc_material_manager& p_material_manager, physics::bc_rigid_static& p_rigid_static, const bc_height_map& p_height_map)
	{
		const auto l_px_height_field = p_height_map.get_px_height_field();
		const auto l_px_height_field_shape = physics::bc_shape_height_field
		(
			l_px_height_field, 
			p_height_map.get_xz_multiplier(),
			get_height_field_y_scale()
		);
		const auto l_px_height_field_materials = core::bc_vector_frame<physics::bc_material>(p_height_map.get_materials_count());

		std::transform
		(
			std::begin(p_height_map.get_materials()),
			std::end(p_height_map.get_materials()),
			std::begin(l_px_height_field_materials),
			[](const bc_height_map_material& p_material)
			{
				return p_material.m_collider_material.m_px_material;
			}
		);

		auto l_px_shape = m_physics.create_shape(l_px_height_field_shape, l_px_height_field_materials.data(), p_height_map.get_materials_count(), true);
		l_px_shape->set_query_group(static_cast<physics::bc_query_group>(bc_actor_physics_group::terrain));
		p_rigid_static.attach_shape(l_px_shape.get());
	}

	void bc_physics_system::create_px_shapes_from_mesh(const bc_material_manager& p_material_manager,
	                                                   physics::bc_rigid_body& p_rigid_body,
	                                                   const bc_sub_mesh& p_mesh,
	                                                   const core::bc_json_key_value* p_collider_materials)
	{
		const core::bc_json_key_value l_empty_collider_json;
		const auto& l_mesh_collider = p_mesh.get_mesh_collider();
		const auto& l_collider_materials = p_collider_materials ? *p_collider_materials : l_empty_collider_json;

		for (const bc_mesh_part_collider_entry& l_mesh_part_collider : l_mesh_collider.get_colliders())
		{
			auto l_material = p_material_manager.get_default_collider_material();
			auto l_material_ite = l_collider_materials.find(l_mesh_part_collider.m_name.c_str());

			if(l_material_ite == std::end(l_collider_materials))
			{
				l_material_ite = l_collider_materials.find("*");
			}

			if (l_material_ite != std::end(l_collider_materials))
			{
				core::bc_any& l_material_key = l_material_ite->second;
				auto& l_material_name = l_material_key.as_throw<core::bc_string>();

				l_material = p_material_manager.find_collider_material(l_material_name.c_str());
			}

			auto l_px_shape = m_physics.create_shape(*l_mesh_part_collider.m_shape, l_material.m_px_material, l_mesh_part_collider.m_shape_flags, true);
			if (!l_px_shape->is_valid())
			{
				throw bc_invalid_argument_exception("Invalid mesh px-shape");
			}

			l_px_shape->set_local_pose(l_mesh_part_collider.m_absolute_transform);
			l_px_shape->set_high_detail_query_shape(l_mesh_part_collider.m_high_detail_query_shape);
			if(p_mesh.get_skinned())
			{
				l_px_shape->set_query_group(static_cast<physics::bc_query_group>(bc_actor_physics_group::skinned_mesh));
			}
				
			auto* l_shape_data = alloc_shape_data();
			l_shape_data->m_collider_entry = &l_mesh_part_collider;
			l_shape_data->m_material_name = l_material.m_name;
			l_shape_data->m_collision_particle = l_material.m_collision_particle;
			l_shape_data->m_collision_decal = l_material.m_collision_decal;

			set_game_shape_data(l_px_shape.get(), *l_shape_data);

			p_rigid_body.attach_shape(l_px_shape.get());
		}
	}

	void bc_physics_system::clear_px_shapes_data(const physics::bc_rigid_body& p_px_actor)
	{
		core::bc_vector_frame<physics::bc_shape> l_shapes(p_px_actor.get_shape_count());
		p_px_actor.get_shapes(l_shapes.data(), l_shapes.size());

		for(auto& l_shape : l_shapes)
		{
			auto* l_shape_data = get_game_shape_data(l_shape);
			if(!l_shape_data)
			{
				continue;
			}

			dealloc_shape_data(l_shape_data);
		}
	}

	void bc_physics_system::update(const platform::bc_clock::update_param& p_clock_update_param)
	{
	}

	void bc_physics_system::_initialize()
	{
		m_physics.initialize
		(
			core::bc_make_unique<physics::bc_default_allocator
				<
					core::bc_aligned_allocator<bcBYTE, 16>,
					core::bc_aligned_allocator_frame<bcBYTE, 16>
				>>(),
			core::bc_make_unique<physics::bc_default_task_dispatcher>(),
			core::bc_make_unique<physics::bc_default_logger>(core::bc_get_service<core::bc_logger>())
		);
		m_shape_data_pool.initialize(5000, core::bc_alloc_type::program);
	}

	void bc_physics_system::_destroy()
	{
		m_shape_data_pool.destroy();
		m_physics.destroy();
	}
}
