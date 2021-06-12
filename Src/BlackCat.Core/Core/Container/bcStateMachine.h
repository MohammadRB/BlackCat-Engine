// [06/03/2021 MRB]

#pragma once

#include <tuple>
#include <variant>
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		template<class ...TStates>
		class bc_state_machine;
		
		template<class TMachine>
		class bc_state_transition
		{
		public:
			using machine_t = TMachine;
			template<class ...TStates> friend class bc_state_machine;
			
		public:
			bc_state_transition(const bc_state_transition&) noexcept = default;

			bc_state_transition(bc_state_transition&&) noexcept = default;

			~bc_state_transition() = default;

			bc_state_transition& operator=(const bc_state_transition&) noexcept = default;

			bc_state_transition& operator=(bc_state_transition&&) noexcept = default;
			
			template<class TState>
			static bc_state_transition<TMachine> transfer_to()
			{
				bc_state_transition<TMachine> l_transporter;
				l_transporter.m_state_transporter = bc_delegate<void(TMachine&)>
				(
					&bc_state_transition<TMachine>::template _transfer<TState>
				);

				return l_transporter;
			}

			template<class TState, class TBeforeTransfer>
			static bc_state_transition<TMachine> transfer_to(TBeforeTransfer p_before_transfer)
			{
				bc_state_transition<TMachine> l_transporter;
				l_transporter.m_state_transporter = bc_delegate<void(TMachine&)>
				(
					[p_before_transfer](TMachine& p_machine)
					{
						p_before_transfer(p_machine.template get_state<TState>());
						bc_state_transition<TMachine>::template _transfer<TState>(p_machine);
					}
				);

				return l_transporter;
			}

			static bc_state_transition<TMachine> empty()
			{
				bc_state_transition<TMachine> l_transporter;
				l_transporter.m_state_transporter = bc_delegate<void(TMachine&)>(&bc_state_transition<TMachine>::template _transfer<void>);

				return l_transporter;
			}
		
		private:
			bc_state_transition() = default;

			void transfer_state(TMachine& p_machine)
			{
				m_state_transporter(p_machine);
			}
			
			template<class TState>
			static void _transfer(TMachine& p_machine)
			{
				p_machine.template transfer_state<TState>();
			}

			template<>
			static void _transfer<void>(TMachine&)
			{
			}

			bc_delegate<void(TMachine&)> m_state_transporter;
		};

		template<class TMachine, class TEvent>
		class bc_state_event_driver
		{
			using state_transition = bc_state_transition<TMachine>;

		private:
			virtual state_transition handle(TEvent& p_event) = 0;
		};
		
		template<class TMachine, class ...TEvents>
		class bc_state : bc_state_event_driver<TMachine, TEvents>...
		{
		protected:
			using state_transition = bc_state_transition<TMachine>;
			template<class ...TStates> friend class bc_state_machine;
			
		protected:
			bc_state() = default;

			bc_state(const bc_state&) noexcept = default;

			~bc_state() = default;

			bc_state& operator=(const bc_state&) noexcept = default;
		
		private:
			template<class TEvent>
			state_transition process_event(TEvent& p_event)
			{
				return handle(p_event);
			}

			void process_enter()
			{
				on_enter();
			}

			void process_exit()
			{
				on_exit();
			}

			template<class TEvent>
			state_transition handle(TEvent&)
			{
				// ignore all not interested events
				return state_transition::empty();
			}

			virtual void on_enter()
			{
			}

			virtual void on_exit()
			{
			}
		};

		template<class TState>
		class bc_state_machine_transition_driver
		{
			template<class ...TStates> friend class bc_state_machine;
			
		private:
			virtual void on_enter(TState& p_state)
			{
			}

			virtual void on_exit(TState& p_state)
			{
			}
		};
		
		template<class ...TStates>
		class bc_state_machine : bc_state_machine_transition_driver<TStates>...
		{
			using bc_state_machine_transition_driver<TStates>::on_enter...;
			using bc_state_machine_transition_driver<TStates>::on_exit...;
			
		public:
			explicit bc_state_machine(TStates... p_states)
				: m_states(std::make_tuple(std::move(p_states)...))
			{
				m_current_state = &std::get<0>(m_states);
			}

			bc_state_machine(bc_state_machine&&) noexcept = default;

			~bc_state_machine() = default;

			bc_state_machine& operator=(bc_state_machine&&) noexcept = default;

			template<class TState>
			TState& get_state()
			{
				return std::get<TState>(m_states);
			}
			
			template<class TEvent>
			void process_event(TEvent& p_event)
			{
				std::visit
				(
					[this, &p_event](auto p_state)
					{
						auto l_state_transition = p_state->process_event(p_event);
						l_state_transition.transfer_state(static_cast<typename decltype(l_state_transition)::machine_t&>(*this));
					},
					m_current_state
				);
			}

			template<class TState>
			void transfer_state()
			{
				std::visit
				(
					[this](auto p_state)
					{
						p_state->process_exit();
						on_exit(*p_state);
					},
					m_current_state
				);

				m_current_state = &get_state<TState>();

				std::visit
				(
					[this](auto p_state)
					{
						p_state->process_enter();
						on_enter(*p_state);
					},
					m_current_state
				);
			}
			
		private:
			std::tuple<TStates...> m_states;
			std::variant<TStates*...> m_current_state;
		};
	}
}