// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Body/bcRigidBody.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_rigid_body_component : public bci_actor_abstract_component
		{
			BC_ABSTRACT_COMPONENT(rgd_bdy)

		public:
			virtual ~bc_rigid_body_component() override;

			virtual physics::bc_actor_type get_body_type() const noexcept = 0;
			
			virtual physics::bc_rigid_body get_body() const noexcept = 0;

			/**
			 * \brief Enable or disable simulation of physics actor
			 * \n bc_actor_flag::disable_gravity and bc_actor_flag::disable_simulation are affected
			 * \n Scene lock will be acquired
			 * \param p_enable 
			 */
			void set_enable(bool p_enable) noexcept;

			/**
			 * \brief Enable or disable kinematic mode for the actor
			 * * \n Scene lock will be acquired
			 * \param p_enable 
			 */
			void set_kinematic(bool p_enable) noexcept;

			void lock();

			void lock_shared();

			void unlock();

			void unlock_shared();
			
		protected:
			bc_rigid_body_component() noexcept;

			bc_rigid_body_component(bc_rigid_body_component&&) noexcept;

			bc_rigid_body_component& operator=(bc_rigid_body_component&&) noexcept;

			physics::bc_scene* get_scene() noexcept;
			
			void added_to_scene(physics::bc_scene& p_scene, physics::bc_rigid_body& p_body);
			
			void remove_from_scene(physics::bc_scene& p_scene, physics::bc_rigid_body& p_body);
			
			void update_px_shape_transforms(physics::bc_rigid_body& p_px_actor, const bc_sub_mesh_px_transform& p_model_space_transforms);
			
			void debug_draw(physics::bc_rigid_body& p_px_actor, const bc_actor_component_debug_draw_context& p_context);

		private:
			physics::bc_scene* m_scene;
		};

		class bc_rigid_component_lock
		{
		public:
			explicit bc_rigid_component_lock(bc_rigid_body_component& p_component);

			bc_rigid_component_lock(const bc_rigid_component_lock&) = delete;

			~bc_rigid_component_lock();

			bc_rigid_component_lock& operator=(const bc_rigid_component_lock&) = delete;

		private:
			bc_rigid_body_component& m_component;
		};

		class bc_rigid_component_shared_lock
		{
		public:
			explicit bc_rigid_component_shared_lock(bc_rigid_body_component& p_component);

			bc_rigid_component_shared_lock(const bc_rigid_component_shared_lock&) = delete;

			~bc_rigid_component_shared_lock();

			bc_rigid_component_shared_lock& operator=(const bc_rigid_component_shared_lock&) = delete;

		private:
			bc_rigid_body_component& m_component;
		};
		
		inline bc_rigid_body_component::bc_rigid_body_component() noexcept
			: bci_actor_abstract_component(),
			m_scene(nullptr)
		{
		}

		inline bc_rigid_body_component::bc_rigid_body_component(bc_rigid_body_component&&) noexcept = default;

		inline bc_rigid_body_component::~bc_rigid_body_component() = default;
		
		inline bc_rigid_body_component& bc_rigid_body_component::operator=(bc_rigid_body_component&&) noexcept = default;

		inline void bc_rigid_body_component::lock()
		{
			if (m_scene)
			{
				m_scene->lock();
			}
		}

		inline void bc_rigid_body_component::lock_shared()
		{
			if (m_scene)
			{
				m_scene->lock_shared();
			}
		}

		inline void bc_rigid_body_component::unlock()
		{
			if (m_scene)
			{
				m_scene->unlock();
			}
		}

		inline void bc_rigid_body_component::unlock_shared()
		{
			if(m_scene)
			{
				m_scene->unlock_shared();
			}
		}
		
		inline physics::bc_scene* bc_rigid_body_component::get_scene() noexcept
		{
			return m_scene;
		}

		inline bc_rigid_component_lock::bc_rigid_component_lock(bc_rigid_body_component& p_component)
			: m_component(p_component)
		{
			m_component.lock();
		}

		inline bc_rigid_component_lock::~bc_rigid_component_lock()
		{
			m_component.unlock();
		}

		inline bc_rigid_component_shared_lock::bc_rigid_component_shared_lock(bc_rigid_body_component& p_component)
			: m_component(p_component)
		{
			m_component.lock_shared();
		}

		inline bc_rigid_component_shared_lock::~bc_rigid_component_shared_lock()
		{
			m_component.unlock_shared();
		}
	}
}