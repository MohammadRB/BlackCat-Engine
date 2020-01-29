// [06/16/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_hierarchy_component : public bc_iactor_component
		{
			BC_COMPONENT(hierarchy)

		public:
			explicit bc_hierarchy_component(bc_actor_component_index p_index);

			bc_hierarchy_component(bc_hierarchy_component&&) noexcept;

			~bc_hierarchy_component();

			bc_hierarchy_component& operator=(bc_hierarchy_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			const core::bc_vector< bc_actor >& get_actors() const;

			void add_actor(const bc_actor& p_actor);

			void remove_actor(const bc_actor& p_actor);

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

		private:
			core::bc_vector< bc_actor > m_actors;
		};
	}
}