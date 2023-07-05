// [17/05/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcDeque.h"
#include "Core/Container/bcBitVector.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
	}
	
	namespace game
	{
		class bc_game_system;
		class bc_actor_component_manager_container;

		class bci_actor_component_container
		{
		public:
			virtual ~bci_actor_component_container() = default;

			virtual core::bc_vector_frame<bcUINT32> export_alive_indices() const = 0;

			virtual bci_actor_component* get(bc_actor_component_id p_id) = 0;

			virtual bc_actor_component_id create(bc_actor_id p_actor_id) = 0;

			virtual bc_actor_component_id create_after(bc_actor_id p_actor_id, bc_actor_component_id p_parent_id = bci_actor_component::invalid_id) = 0;

			virtual void remove(bc_actor_component_id p_id) = 0;

			virtual void handle_events(const platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager,
				bc_game_system& p_game_system,
				bc_actor_component_manager_container& p_manager,
				const core::bc_vector_frame<bcUINT32>& p_alive_indices) = 0;

			virtual void update(const platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager,
				bc_game_system& p_game_system,
				bc_actor_component_manager_container& p_manager,
				const core::bc_vector_frame<bcUINT32>& p_alive_indices) = 0;

			virtual bcSIZE size() const noexcept = 0;

		protected:
			bci_actor_component_container() = default;
		};

		template<class TComponent>
		class bc_actor_component_container : public bci_actor_component_container
		{
		public:
			bc_actor_component_container();

			bc_actor_component_container(bc_actor_component_container&&) noexcept;

			~bc_actor_component_container() override;

			bc_actor_component_container& operator=(bc_actor_component_container&&) noexcept;

			core::bc_vector_frame<bcUINT32> export_alive_indices() const override;

			TComponent* get(bc_actor_component_id p_id) override;

			bc_actor_component_id create(bc_actor_id p_actor_id) override;

			/**
			 * \brief In case of parent child relative between actors, component of child actor must be inserted after
			 * component of parent actor so it will be updated after parent component
			 * \param p_actor_id 
			 * \param p_parent_id 
			 * \return 
			 */
			bc_actor_component_id create_after(bc_actor_id p_actor_id, bc_actor_component_id p_parent_id) override;

			void remove(bc_actor_component_id p_id) override;

			void handle_events(const platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager,
				bc_game_system& p_game_system,
				bc_actor_component_manager_container& p_manager,
				const core::bc_vector_frame<bcUINT32>& p_alive_components) override;

			void update(const platform::bc_clock::update_param& p_clock,
				core::bc_query_manager& p_query_manager,
				bc_game_system& p_game_system,
				bc_actor_component_manager_container& p_manager,
				const core::bc_vector_frame<bcUINT32>& p_alive_components) override;

			bcSIZE size() const noexcept override;

		private:
			core::bc_bit_vector m_bit_block;
			core::bc_deque<TComponent> m_components;
		};

		template<class TComponent>
		bc_actor_component_container<TComponent>::bc_actor_component_container()
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent must inherit from bc_iactor_component");
		}

		template<class TComponent>
		bc_actor_component_container<TComponent>::bc_actor_component_container(bc_actor_component_container&&) noexcept = default;

		template<class TComponent>
		bc_actor_component_container<TComponent>::~bc_actor_component_container() = default;

		template<class TComponent>
		bc_actor_component_container<TComponent>& bc_actor_component_container<TComponent>::operator=(bc_actor_component_container&&) noexcept = default;

		template<class TComponent>
		TComponent* bc_actor_component_container<TComponent>::get(bc_actor_component_id p_id)
		{
			const auto l_is_set = m_bit_block[p_id];
			if(!l_is_set)
			{
				return nullptr;
			}
			
			auto& l_entry = m_components[p_id];
			return &l_entry;
		}

		template<class TComponent>
		bc_actor_component_id bc_actor_component_container<TComponent>::create(bc_actor_id p_actor_id)
		{
			bcUINT32 l_free_slot;
			const auto l_has_free_slot = m_bit_block.find_first_false(l_free_slot);

			if(l_has_free_slot)
			{
				new (&m_components[l_free_slot]) TComponent(p_actor_id, l_free_slot);
				m_bit_block.make_true(l_free_slot);
			}
			else
			{
				m_components.push_back(TComponent(p_actor_id, l_free_slot));
				l_free_slot = m_components.size() - 1;
				m_bit_block.resize(m_components.size());
				m_bit_block.make_true(l_free_slot);
			}

			return l_free_slot;
		}

		template<class TComponent>
		bc_actor_component_id bc_actor_component_container<TComponent>::create_after(bc_actor_id p_actor_id, bc_actor_component_id p_parent_id)
		{
			bcUINT32 l_free_slot;
			const auto l_has_free_slot = m_bit_block.find_first_false(l_free_slot, p_parent_id);

			if (l_has_free_slot)
			{
				new (&m_components[l_free_slot]) TComponent(p_actor_id, l_free_slot);
				m_bit_block.make_true(l_free_slot);
			}
			else
			{
				m_components.push_back(TComponent(p_actor_id, l_free_slot));
				l_free_slot = m_components.size() - 1;
				m_bit_block.resize(m_components.size());
				m_bit_block.make_true(l_free_slot);
			}

			return l_free_slot;
		}

		template<class TComponent>
		void bc_actor_component_container<TComponent>::remove(bc_actor_component_id p_id)
		{
			// First destroy component then modify its bit flag because components may want to do some cleanup
			// which we must preserve their existence
			m_components[p_id].~TComponent();
			m_bit_block.make_false(p_id);
		}

		template<class TComponent>
		bcSIZE bc_actor_component_container<TComponent>::size() const noexcept
		{
			return m_components.size();
		}
	}
}