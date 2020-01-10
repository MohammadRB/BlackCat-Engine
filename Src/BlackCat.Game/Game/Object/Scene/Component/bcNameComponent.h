// [11/08/2018 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "PlatformImp/bcIDELogger.h"

namespace  black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_name_component : public bc_iactor_component
		{
			BC_COMPONENT(name)

		public:
			static constexpr const bcCHAR* s_entity_name_json_key = "entity_name";

		public:
			bc_name_component(bc_actor_component_index p_index);

			bc_name_component(bc_name_component&& p_other) noexcept;

			~bc_name_component();

			bc_name_component& operator=(bc_name_component&& p_other) noexcept;

			void set_entity_name(const bcCHAR* p_entity_name);

			const bcCHAR* get_entity_name() const;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters) override;

		protected:

		private:
			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			void handle_event(const bc_actor& p_actor, const bc_actor_event& p_event) override;

			const bcCHAR* m_entity_name;
		};
	}
}