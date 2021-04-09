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

			virtual physics::bc_rigid_body& get_body() noexcept = 0;

			/**
			 * \brief Enable or disable simulation of physics actor
			 * \param p_enable 
			 */
			void set_enable(bool p_enable) noexcept;
			
		protected:
			bc_rigid_body_component() noexcept;

			bc_rigid_body_component(bc_rigid_body_component&&) noexcept;

			bc_rigid_body_component& operator=(bc_rigid_body_component&&) noexcept;

			physics::bc_scene* get_scene() noexcept;
			
			void added_to_scene(physics::bc_scene& p_scene);
			
			void remove_from_scene(physics::bc_scene& p_scene);
			
			void update_px_shape_transforms(physics::bc_rigid_body& p_px_actor, const bc_sub_mesh_px_transform& p_model_space_transforms);

			void debug_draw(physics::bc_rigid_body& p_px_actor, const bc_actor_component_debug_draw_context& p_context);

		private:
			physics::bc_scene* m_scene;
		};

		inline bc_rigid_body_component::bc_rigid_body_component() noexcept
			: bci_actor_abstract_component(),
			m_scene(nullptr)
		{
		}

		inline bc_rigid_body_component::bc_rigid_body_component(bc_rigid_body_component&&) noexcept = default;

		inline bc_rigid_body_component::~bc_rigid_body_component() = default;
		
		inline bc_rigid_body_component& bc_rigid_body_component::operator=(bc_rigid_body_component&&) noexcept = default;

		inline physics::bc_scene* bc_rigid_body_component::get_scene() noexcept
		{
			return m_scene;
		}
	}
}