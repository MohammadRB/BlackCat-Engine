// [12/21/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

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
			core::bc_unique_ptr< bcINT16 > l_buffer(static_cast< bcINT16* >(bcAlloc(sizeof(bcINT16) * l_num_samples, core::bc_alloc_type::frame)));
			bcINT16* l_buffer_ptr = l_buffer.get();

			for (bcUINT32 l_index = 0; l_index < l_num_samples; ++l_index)
			{
				l_buffer_ptr[l_index] = height_to_int16(p_samples[l_index]);
			}

			return l_buffer;
		}

		void bc_physics_system::connect_px_actor_to_game_actor(physics::bc_actor& p_px_actor, bc_actor p_actor)
		{
			p_px_actor.set_data(reinterpret_cast<void*>(static_cast<bcINTPTR>(p_actor.get_index())));
		}

		bc_actor bc_physics_system::get_game_actor(physics::bc_actor& p_px_actor)
		{
			return bc_actor(static_cast<bc_actor_index>(reinterpret_cast<bcINTPTR>(p_px_actor.get_data())));
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
