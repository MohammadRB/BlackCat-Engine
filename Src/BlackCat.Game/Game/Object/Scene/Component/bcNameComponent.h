// [11/08/2018 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "PlatformImp/bc_ide_logger.h"

namespace  black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_name_component : public bc_iactor_component
		{
			BC_COMPONENT(name)

		public:
			bc_name_component(bc_actor_component_index p_index);

			bc_name_component(bc_name_component&& p_other) noexcept;

			~bc_name_component();

			bc_name_component& operator=(bc_name_component&& p_other) noexcept;

			void set_entity_name(const bcCHAR* p_entity_name);

			const bcCHAR* get_entity_name() const;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

		protected:

		private:
			const bcCHAR* m_entity_name;
		};
	}
}