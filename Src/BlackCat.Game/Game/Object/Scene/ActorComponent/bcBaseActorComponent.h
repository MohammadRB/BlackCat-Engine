// [10/12/2023 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentEventAdapter.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL _bc_base_actor_component : public bci_actor_component
	{
	protected:
		_bc_base_actor_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		_bc_base_actor_component(_bc_base_actor_component&&) noexcept = default;

		~_bc_base_actor_component() override = default;

		_bc_base_actor_component& operator=(_bc_base_actor_component&&) noexcept = default;

		const core::bc_matrix4f& get_transform() const noexcept;

		core::bc_vector3f get_translation() const noexcept;
	};

	template<class TComponent>
	class bc_base_actor_component : public _bc_base_actor_component
	{
	public:
		void handle_event(const bc_actor_component_event_context& p_context) override final;

	protected:
		bc_base_actor_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

		bc_base_actor_component(bc_base_actor_component&&) noexcept = default;

		~bc_base_actor_component() override = default;

		bc_base_actor_component& operator=(bc_base_actor_component&&) noexcept = default;
	};

	template <class TComponent>
	bc_base_actor_component<TComponent>::bc_base_actor_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
		: _bc_base_actor_component(p_actor_id, p_id)
	{
	}

	template <class TComponent>
	void bc_base_actor_component<TComponent>::handle_event(const bc_actor_component_event_context& p_context)
	{
		if constexpr (std::is_base_of_v<bc_component_event_dispatcher, TComponent>)
		{
			auto& l_component = static_cast<TComponent&>(*this);
			auto& l_event_dispatcher = static_cast<bc_component_event_dispatcher&>(l_component);
			l_event_dispatcher.dispatch_event(p_context);
		}
	}
}
