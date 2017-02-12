// [06/16/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;

		class BC_GAME_DLL bc_hierarchy_component : public bc_iactor_component
		{
			BC_COMPONENT(hierarchy)

		public:
			explicit bc_hierarchy_component(bc_actor_component_index p_index);

			bc_hierarchy_component(bc_hierarchy_component&&) noexcept;

			~bc_hierarchy_component();

			bc_hierarchy_component& operator=(bc_hierarchy_component&&) noexcept;

			const core::bc_vector< bc_actor >& get_actors() const
			{
				return m_actors;
			}

			bc_actor get_actor() const noexcept override;

			void add_actor(const bc_actor& p_actor);

			void remove_actor(const bc_actor& p_actor);

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param) override;

		protected:

		private:
			core::bc_vector< bc_actor > m_actors;
		};
	}
}
