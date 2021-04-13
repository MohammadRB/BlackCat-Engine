// [02/11/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "PhysicsImp/Fundation/bcMaterial.h"

namespace black_cat
{
	namespace game
	{
		class bc_material_manager;

		class bc_collider_material_description
		{
		public:
			using hash_t = std::hash< const bcCHAR* >::result_type;

		public:
			hash_t m_hash;
			const bcCHAR* m_name;
			physics::bc_material m_px_material;
			const bcCHAR* m_collision_particle;
			const bcCHAR* m_collision_decal;
		};
		
		class bc_collider_material
		{
			friend class bc_material_manager;
			
		public:
			bc_collider_material(bc_collider_material&&) noexcept;

			~bc_collider_material();

			bc_collider_material& operator=(bc_collider_material&&) noexcept;

			const bcCHAR* get_name() const noexcept;
			
			physics::bc_material get_px_material() const noexcept;

			const bcCHAR* get_particle_name() const noexcept;
			
			const bcCHAR* get_decal_name() const noexcept;

		private:
			bc_collider_material(core::bc_string_program p_name,
				physics::bc_material_ref p_px_material, 
				core::bc_string_program p_particle, 
				core::bc_string_program p_decal);

			core::bc_string_program m_name;
			physics::bc_material_ref m_px_material;
			core::bc_string_program m_particle;
			core::bc_string_program m_decal;
			const bcCHAR* m_particle_ptr;
			const bcCHAR* m_decal_ptr;
		};

		inline bc_collider_material::bc_collider_material(bc_collider_material&& p_other) noexcept
			: m_name(std::move(p_other.m_name)),
			m_px_material(std::move(p_other.m_px_material)),
			m_particle(std::move(p_other.m_particle)),
			m_decal(std::move(p_other.m_decal))
		{
			m_particle_ptr = m_particle.empty() ? nullptr : m_particle.c_str();
			m_decal_ptr = m_decal.empty() ? nullptr : m_decal.c_str();
		}

		inline bc_collider_material::~bc_collider_material() = default;

		inline bc_collider_material& bc_collider_material::operator=(bc_collider_material&& p_other) noexcept
		{
			m_name = std::move(p_other.m_name);
			m_px_material = std::move(p_other.m_px_material);
			m_particle = std::move(p_other.m_particle);
			m_decal = std::move(p_other.m_decal);
			m_particle_ptr = m_particle.empty() ? nullptr : m_particle.c_str();
			m_decal_ptr = m_decal.empty() ? nullptr : m_decal.c_str();
			
			return *this;
		}

		inline const bcCHAR* bc_collider_material::get_name() const noexcept
		{
			return m_name.c_str();
		}

		inline physics::bc_material bc_collider_material::get_px_material() const noexcept
		{
			return m_px_material.get();
		}

		inline const bcCHAR* bc_collider_material::get_particle_name() const noexcept
		{
			return m_particle_ptr;
		}

		inline const bcCHAR* bc_collider_material::get_decal_name() const noexcept
		{
			return m_decal_ptr;
		}

		inline bc_collider_material::bc_collider_material(core::bc_string_program p_name, 
			physics::bc_material_ref p_px_material, 
			core::bc_string_program p_particle, 
			core::bc_string_program p_decal)
			: m_name(std::move(p_name)),
			m_px_material(std::move(p_px_material)),
			m_particle(std::move(p_particle)),
			m_decal(std::move(p_decal))
		{
			m_particle_ptr = m_particle.empty() ? nullptr : m_particle.c_str();
			m_decal_ptr = m_decal.empty() ? nullptr : m_decal.c_str();
		}
	}
}