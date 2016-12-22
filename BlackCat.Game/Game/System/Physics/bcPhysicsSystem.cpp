// [12/21/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Physics/bcPhysicsSystem.h"

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
				core::bc_make_unique< physics::bc_default_logger >(core::bc_get_service<core::bc_logger>())
			);
		}

		void bc_physics_system::_destroy()
		{
			m_physics.destroy();
		}
	}
}