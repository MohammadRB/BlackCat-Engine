// [12/21/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_physics_system : public core::bc_initializable<>
		{
		public:
			bc_physics_system();

			bc_physics_system(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);

			~bc_physics_system();

			bc_physics_system& operator=(bc_physics_system&&) noexcept(core::bc_type_traits<physics::bc_physics>::is_no_throw_move);;

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:
			void _initialize() override;

			void _destroy() override;

		private:
			physics::bc_physics m_physics;
		};
	}
}