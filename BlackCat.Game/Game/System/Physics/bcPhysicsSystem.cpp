// [12/21/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcAllocator.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "PhysicsImp/Fundation/bcMeshBuffer.h"
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

		physics::bc_mesh_buffer bc_physics_system::create_height_field(bcUINT32 p_num_row, bcUINT32 p_num_column, bcFLOAT* p_samples)
		{
			bcUINT32 l_num_samples = p_num_row * p_num_column;
			core::bc_unique_ptr<bcINT16> l_buffer(static_cast< bcINT16* >(bcAlloc(sizeof(bcINT16) * l_num_samples, core::bc_alloc_type::frame)));
			bcINT16* l_buffer_ptr = l_buffer.get();

			for(bcUINT32 l_index = 0; l_index < l_num_samples; ++l_index)
			{
				l_buffer_ptr[l_index] = height_to_int16(p_samples[l_index]);
			}

			physics::bc_strided_data l_samples_data(l_buffer_ptr, sizeof(bcINT16));
			physics::bc_strided_data l_samples_material;

			physics::bc_height_field_desc l_desc(p_num_row, p_num_column, l_samples_data, l_samples_material);
			auto l_height_field_buffer = m_physics.create_height_field(l_desc);

			return l_height_field_buffer;
		}

		physics::bc_height_field_ref bc_physics_system::create_height_field(physics::bc_mesh_buffer& p_buffer)
		{
			return m_physics.create_height_field(p_buffer);
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