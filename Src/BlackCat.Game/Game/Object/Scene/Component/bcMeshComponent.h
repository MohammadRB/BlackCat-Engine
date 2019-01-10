// [04/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_component : public bc_iactor_component
		{
			BC_COMPONENT(mesh)

		public:
			explicit bc_mesh_component(bc_actor_component_index p_index);

			bc_mesh_component(bc_mesh_component&&) noexcept;

			~bc_mesh_component();

			bc_mesh_component& operator=(bc_mesh_component&&) noexcept;

			const bc_sub_mesh& get_mesh() const
			{
				return m_sub_mesh;
			}

			const bc_sub_mesh_transformation& get_mesh_part_transformation() const
			{
				return m_mesh_part_transformation;
			}

			bc_actor get_actor() const noexcept override;

			void set_world_transform(const core::bc_matrix4f& p_transform);

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			void render(const bc_actor& p_actor, const bc_render_component& p_render_component) const;

		protected:

		private:
			bc_sub_mesh m_sub_mesh;
			bc_sub_mesh_transformation m_mesh_part_transformation;
		};
	}
}