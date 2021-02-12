// [02/11/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "PhysicsImp/Fundation/bcMaterial.h"

namespace black_cat
{
	namespace game
	{
		class bc_material_manager;
		
		class bc_collider_material
		{
			friend class bc_material_manager;
			
		public:
			bc_collider_material(bc_collider_material&&) noexcept;

			~bc_collider_material();

			bc_collider_material& operator=(bc_collider_material&&) noexcept;

			physics::bc_material get_px_material() const noexcept;

			const bcCHAR* get_particle_name() const noexcept;

		private:
			bc_collider_material(physics::bc_material_ref p_px_material, core::bc_string_program p_collision_particle);
			
			physics::bc_material_ref m_px_material;
			core::bc_string_program m_collision_particle;
		};

		inline bc_collider_material::bc_collider_material(bc_collider_material&&) noexcept = default;

		inline bc_collider_material::~bc_collider_material() = default;

		inline bc_collider_material& bc_collider_material::operator=(bc_collider_material&&) noexcept = default;

		inline physics::bc_material bc_collider_material::get_px_material() const noexcept
		{
			return m_px_material.get();
		}

		inline const bcCHAR* bc_collider_material::get_particle_name() const noexcept
		{
			return m_collision_particle.c_str();
		}

		inline bc_collider_material::bc_collider_material(physics::bc_material_ref p_px_material, core::bc_string_program p_collision_particle)
			: m_px_material(std::move(p_px_material)),
			m_collision_particle(std::move(p_collision_particle))
		{
		}
	}
}