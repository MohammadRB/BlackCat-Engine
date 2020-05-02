// [04/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_component : public bc_render_component
		{
			BC_ABSTRACT_COMPONENT(mesh)
			
		public:
			explicit bc_mesh_component(bc_actor_component_index p_index);

			bc_mesh_component(bc_mesh_component&&) noexcept;

			~bc_mesh_component();

			bc_mesh_component& operator=(bc_mesh_component&&) noexcept;

			const bc_sub_mesh& get_mesh() const;

			const bc_sub_mesh_transformation& get_mesh_transformation() const;

			core::bc_vector3f get_world_position() const;

			core::bc_matrix4f get_world_transform() const;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void render(bc_render_state_buffer& p_buffer) const override;

		protected:
			void set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform);
			
		private:
			bc_sub_mesh m_sub_mesh;
			bc_sub_mesh_transformation m_mesh_part_transformation;
		};

		inline const bc_sub_mesh& bc_mesh_component::get_mesh() const
		{
			return m_sub_mesh;
		}

		inline const bc_sub_mesh_transformation& bc_mesh_component::get_mesh_transformation() const
		{
			return m_mesh_part_transformation;
		}
	}
}