// [17/01/2024 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"
#include <type_traits>

namespace black_cat::game
{
	template<class TEvent>
	struct bc_component_event_context : public bc_actor_component_event_context
	{
		bc_component_event_context(const bc_actor_component_event_context& p_context, const TEvent& p_event)
			: bc_actor_component_event_context(p_context)
			, m_event(p_event)
		{
		}

		const TEvent& m_event;
	};

	template<class ...>
	class _bc_component_event_adapter;

	template<>
	class BC_GAME_DLL _bc_component_event_adapter<>
	{
	protected:
		_bc_component_event_adapter() = default;

		_bc_component_event_adapter(const _bc_component_event_adapter&) = default;

		~_bc_component_event_adapter() = default;

		_bc_component_event_adapter& operator=(const _bc_component_event_adapter&) = default;

		void on_event() = delete;
	};

	template<class TEvent, class ...TEvents>
	class _bc_component_event_adapter<TEvent, TEvents...> : public _bc_component_event_adapter<TEvents...>
	{
		using base = _bc_component_event_adapter<TEvents...>;
		
	protected:
		using base::on_event;

		_bc_component_event_adapter()
		{
			static_assert(std::is_base_of_v<bc_actor_event, TEvent>);
		}

		_bc_component_event_adapter(const _bc_component_event_adapter&) = default;

		~_bc_component_event_adapter() = default;

		_bc_component_event_adapter& operator=(const _bc_component_event_adapter&) = default;

		virtual void on_event(const bc_component_event_context<TEvent>& p_context) = 0;
	};

	class bc_component_event_dispatcher
	{
	public:
		virtual void dispatch_event(const bc_actor_component_event_context& p_context) = 0;

	protected:
		bc_component_event_dispatcher() = default;

		bc_component_event_dispatcher(const bc_component_event_dispatcher&) = default;

		~bc_component_event_dispatcher() = default;

		bc_component_event_dispatcher& operator=(const bc_component_event_dispatcher&) = default;
	};

	template<class ...TEvents>
	class bc_component_event_adapter : public bc_component_event_dispatcher, _bc_component_event_adapter<TEvents...>
	{
	protected:
		bc_component_event_adapter() = default;

		bc_component_event_adapter(const bc_component_event_adapter&) = default;

		~bc_component_event_adapter() = default;

		bc_component_event_adapter& operator=(const bc_component_event_adapter&) = default;

		void dispatch_event(const bc_actor_component_event_context& p_context) override final
		{
			auto l_expansion_list =
			{
				(
					[this, p_context]()
					{
						if (const auto* l_event = core::bci_message::as<TEvents>(p_context.m_event))
						{
							this->on_event(bc_component_event_context<TEvents>(p_context, *l_event));
						}
						return true;
					}()
				)...
			};
		}
	};
}
