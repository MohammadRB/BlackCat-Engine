// [06/07/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_render_component::bc_render_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_render_system(nullptr)
		{
		}

		bc_render_component::bc_render_component(bc_render_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_render_system(p_other.m_render_system)
		{
		}

		bc_render_component::~bc_render_component()
		{	
		}

		bc_render_component& bc_render_component::operator=(bc_render_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));
			m_render_system = p_other.m_render_system;

			return *this;
		}

		void bc_render_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			m_render_system = &core::bc_service_manager::get().get_service< game::bc_game_system >()->get_render_system();
		}

		void bc_render_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_render_component::render(const bc_render_state* p_render_state, const bc_render_instance& p_render_instance) const
		{
			m_render_system->add_render_instance(p_render_state, p_render_instance);
		}
	}
}