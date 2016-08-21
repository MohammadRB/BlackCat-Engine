// [04/29/2016 MRB]

#pragma once

#include "Graphic/Math/bcVector3f.h"
#include "Graphic/Math/bcMatrix3f.h"
#include "Graphic/Math/bcMatrix4f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scence/bcActor.h"
#include "Game/Object/Scence/bcActorComponent.h"
#include "Game/Object/Mesh/bcMeshPart.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_component : public bc_iactor_component
		{
		public:
			explicit bc_mesh_component(bc_actor_component_index p_index);

			bc_mesh_component(bc_mesh_component&&) noexcept;

			~bc_mesh_component();

			bc_mesh_component& operator=(bc_mesh_component&&) noexcept;

			const bc_mesh_part& get_mesh_part() const
			{
				return m_mesh_part;
			}

			const bc_mesh_part_transformation& get_mesh_part_transformation() const
			{
				return m_mesh_part_transformation;
			}

			void initialize(const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param) override;

			static constexpr const bcCHAR* component_name()
			{
				return core::g_cmp_mesh;
			}

		protected:

		private:
			bc_mesh_part m_mesh_part;
			bc_mesh_part_transformation m_mesh_part_transformation;
		};
	}
}