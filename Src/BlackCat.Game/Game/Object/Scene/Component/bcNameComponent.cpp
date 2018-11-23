// [11/08/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "PlatformImp/bc_ide_logger.h"

namespace black_cat
{
	namespace game
	{
		bc_name_component::bc_name_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_entity_name(nullptr)
		{
		}

		bc_name_component::bc_name_component(bc_name_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_entity_name(p_other.m_entity_name)
		{
		}

		bc_name_component::~bc_name_component()
		{
		}

		bc_name_component& bc_name_component::operator=(bc_name_component&& p_other) noexcept
		{
			m_entity_name = p_other.m_entity_name;
			bc_iactor_component::operator=(std::move(p_other));

			return *this;
		}

		void bc_name_component::set_entity_name(const bcCHAR* p_entity_name)
		{
			m_entity_name = p_entity_name;
		}

		const bcCHAR* bc_name_component::get_entity_name() const
		{
			return m_entity_name;
		}

		bc_actor bc_name_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		void bc_name_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
		}

		void bc_name_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}
	}
}
