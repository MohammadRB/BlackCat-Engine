// [12/21/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_physics_system::bc_physics_system() = default;

		bc_physics_system::bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move) = default;

		bc_physics_system::~bc_physics_system() = default;

		bc_physics_system& bc_physics_system::operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move) = default;

		core::bc_unique_ptr<bcINT16> bc_physics_system::convert_height_field_samples(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples)
		{
			const bcUINT32 l_num_samples = p_num_row * p_num_column;
			core::bc_unique_ptr< bcINT16 > l_buffer(static_cast< bcINT16* >(BC_ALLOC(sizeof(bcINT16) * l_num_samples, core::bc_alloc_type::frame)));
			bcINT16* l_buffer_ptr = l_buffer.get();

			for (bcUINT32 l_index = 0; l_index < l_num_samples; ++l_index)
			{
				l_buffer_ptr[l_index] = height_to_int16(p_samples[l_index]);
			}

			return l_buffer;
		}

		void bc_physics_system::create_px_shapes_from_height_map(bc_material_manager& p_material_manager, 
			physics::bc_rigid_static& p_rigid_static, 
			const bc_height_map_component& p_height_map)
		{
			const auto& l_height_map = p_height_map.get_height_map();
			const auto l_px_height_field = p_height_map.get_height_map().get_px_height_field();
			const auto l_px_height_field_shape = physics::bc_shape_height_field
			(
				l_px_height_field, 
				p_height_map.get_height_map().get_xz_multiplier(), 
				get_height_field_y_scale()
			);
			const auto l_px_height_field_materials = core::bc_vector_frame<physics::bc_material>(l_height_map.get_materials_count());

			std::transform
			(
				l_height_map.get_materials(),
				l_height_map.get_materials() + l_height_map.get_materials_count(),
				std::begin(l_px_height_field_materials),
				[](const bc_height_map_material& p_material)
				{
					return p_material.m_collider_material.m_px_material;
				}
			);

			p_rigid_static.create_shape(l_px_height_field_shape, l_px_height_field_materials.data(), l_height_map.get_materials_count())
			              .set_query_group(static_cast< physics::bc_query_group >(bc_actor_group::terrain));
		}

		void bc_physics_system::create_px_shapes_from_mesh(bc_material_manager& p_material_manager,
			physics::bc_rigid_body& p_px_actor,
			const bc_mesh_component& p_mesh,
			const core::bc_json_key_value* p_collider_materials)
		{
			const core::bc_json_key_value l_empty_collider_json;
			const auto& l_mesh = p_mesh.get_mesh();
			const auto& l_collider_materials = p_collider_materials ? *p_collider_materials : l_empty_collider_json;

			for (const auto& l_mesh_part_collider : l_mesh.get_mesh_collider())
			{
				auto l_material = p_material_manager.get_default_collider_material();
				const auto l_material_ite = l_collider_materials.find(l_mesh_part_collider.first.c_str());
				if (l_material_ite != std::end(l_collider_materials))
				{
					core::bc_any& l_material_key = l_material_ite->second;
					auto& l_material_name = l_material_key.as_throw< core::bc_string >();

					l_material = p_material_manager.find_collider_material(l_material_name.c_str());
				}
				
				for (bc_mesh_part_collider_entry& l_collider_entry : l_mesh_part_collider.second)
				{
					auto l_px_shape = p_px_actor.create_shape(*l_collider_entry.m_shape, l_material.m_px_material, l_collider_entry.m_shape_flags);

					BC_ASSERT(l_px_shape.is_valid());

					l_px_shape.set_local_pose(l_collider_entry.m_initial_transform);
					l_px_shape.set_high_detail_query_shape(l_collider_entry.m_high_detail_query_shape);
					if(l_mesh.get_skinned())
					{
						l_px_shape.set_query_group(static_cast<physics::bc_query_group>(bc_actor_group::skinned_mesh));
					}
					
					auto* l_shape_data = m_shape_data_pool.alloc();
					l_shape_data->m_collider_entry = &l_collider_entry;
					l_shape_data->m_material_name = l_material.m_name;
					l_shape_data->m_collision_particle = l_material.m_collision_particle;
					l_shape_data->m_collision_decal = l_material.m_collision_decal;
					
					l_px_shape.set_data(l_shape_data);
				}
			}
		}

		void bc_physics_system::clear_px_shapes_data(const physics::bc_rigid_body& p_px_actor)
		{
			core::bc_vector_frame< physics::bc_shape > l_shapes(p_px_actor.get_shape_count());
			p_px_actor.get_shapes(l_shapes.data(), l_shapes.size());

			for(auto& l_shape : l_shapes)
			{
				auto* l_shape_data = static_cast<bc_px_shape_data*>(l_shape.get_data());
				if(!l_shape_data)
				{
					continue;
				}

				m_shape_data_pool.free(l_shape_data);
			}
		}

		void bc_physics_system::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_physics_system::_initialize()
		{
			m_physics.initialize
			(
				core::bc_make_unique< physics::bc_default_allocator
				<
					core::bc_aligned_allocator< bcBYTE, 16 >,
					core::bc_aligned_allocator_frame< bcBYTE, 16 >
				> >(),
				core::bc_make_unique< physics::bc_default_task_dispatcher >(),
				core::bc_make_unique< physics::bc_default_logger >(core::bc_get_service<core::bc_logger>())
			);
			m_shape_data_pool.initialize(5000, core::bc_alloc_type::program);
		}

		void bc_physics_system::_destroy()
		{
			m_shape_data_pool.destroy();
			m_physics.destroy();
		}
	}
}