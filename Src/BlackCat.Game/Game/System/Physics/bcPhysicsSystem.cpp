// [12/21/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_physics_system::bc_physics_system()
		{
		}

		bc_physics_system::bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move)
		{
		}

		bc_physics_system::~bc_physics_system()
		{
		}

		bc_physics_system& bc_physics_system::operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move)
		{
			return *this;
		}

		core::bc_unique_ptr< bcINT16 > bc_physics_system::convert_height_field_samples(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples)
		{
			bcUINT32 l_num_samples = p_num_row * p_num_column;
			core::bc_unique_ptr< bcINT16 > l_buffer(static_cast< bcINT16* >(BC_ALLOC(sizeof(bcINT16) * l_num_samples, core::bc_alloc_type::frame)));
			bcINT16* l_buffer_ptr = l_buffer.get();

			for (bcUINT32 l_index = 0; l_index < l_num_samples; ++l_index)
			{
				l_buffer_ptr[l_index] = height_to_int16(p_samples[l_index]);
			}

			return l_buffer;
		}

		void bc_physics_system::set_game_actor(physics::bc_actor& p_px_actor, bc_actor p_actor) noexcept
		{
			p_px_actor.set_data(reinterpret_cast<void*>(static_cast<bcINTPTR>(p_actor.get_index())));
		}

		bc_actor bc_physics_system::get_game_actor(physics::bc_actor& p_px_actor) noexcept
		{
			return bc_actor(static_cast<bc_actor_index>(reinterpret_cast<bcINTPTR>(p_px_actor.get_data())));
		}

		void bc_physics_system::create_px_shapes_from_height_map(physics::bc_rigid_static& p_rigid_static,
			const bc_actor& p_actor,
			const bc_height_map_component& p_height_map)
		{
			const auto l_px_height_field = p_height_map.get_height_map().get_px_height_field();

			const auto l_height_field_shape = physics::bc_shape_height_field
			(
				l_px_height_field, p_height_map.get_height_map().get_xz_multiplier(), get_height_field_y_scale()
			);
			auto l_height_field_material = m_physics.create_material(1, 1, 0.1f);

			p_rigid_static.create_shape(l_height_field_shape, l_height_field_material.get())
			              .set_query_group(static_cast< physics::bc_query_group >(bc_query_group::terrain));
		}

		void bc_physics_system::create_px_shapes_from_mesh(physics::bc_rigid_body& p_px_actor,
			const bc_actor& p_actor,
			const bc_mesh_component& p_mesh)
		{
			const auto& l_mesh = p_mesh.get_mesh();
			auto l_px_material = m_physics.create_material(1, 1, 0.1f);

			for (const auto& l_mesh_collider : l_mesh.get_mesh_collider())
			{
				for (bc_mesh_part_collider_entry& l_collider_entry : l_mesh_collider.second)
				{
					auto l_px_shape = p_px_actor.create_shape(*l_collider_entry.m_px_shape, l_px_material.get(), l_collider_entry.m_shape_flags);

					BC_ASSERT(l_px_shape.is_valid());

					l_px_shape.set_local_pose(l_collider_entry.m_initial_transform);
					l_px_shape.set_data(&l_collider_entry);

					if(l_mesh.get_skinned())
					{
						l_px_shape.set_query_group(static_cast<physics::bc_query_group>(bc_query_group::skinned_mesh));
					}
				}
			}
		}

		void bc_physics_system::update(core_platform::bc_clock::update_param p_clock_update_param)
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
		}

		void bc_physics_system::_destroy()
		{
			m_physics.destroy();
		}
	}
}