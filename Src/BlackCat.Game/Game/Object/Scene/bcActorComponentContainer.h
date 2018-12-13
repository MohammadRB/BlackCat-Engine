// [05/17/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcNullable.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "PlatformImp/bc_ide_logger.h"
#include "PlatformImp/bc_ide_logger.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;

		class bc_iactor_component_container
		{
		public:
			virtual ~bc_iactor_component_container() = default;

			virtual bc_iactor_component* get(bc_actor_component_index p_index) = 0;

			virtual bc_actor_component_index create() = 0;

			virtual bc_actor_component_index create_after(bc_actor_component_index p_parent_index = g_actor_component_invalid_index) = 0;

			virtual void remove(bc_actor_component_index p_index) = 0;

			virtual void update(const bc_actor_component_manager* p_manager, const core_platform::bc_clock::update_param& p_clock_update_param) = 0;

			virtual bcSIZE size() = 0;

			virtual bcSIZE capacity() = 0;

		protected:
			bc_iactor_component_container() = default;

		private:
		};

		template< class TComponent >
		class bc_actor_component_container : public bc_iactor_component_container
		{
		public:
			bc_actor_component_container();

			bc_actor_component_container(bc_actor_component_container&&) noexcept;

			~bc_actor_component_container();

			bc_actor_component_container& operator=(bc_actor_component_container&&) noexcept;

			bc_iactor_component* get(bc_actor_component_index p_index) override;

			bc_actor_component_index create() override;

			// In case of parent child relative between actors, component of child actor must be inserted after
			// component of parent actor so it will be updated after parent component
			bc_actor_component_index create_after(bc_actor_component_index p_parent_index) override;

			void remove(bc_actor_component_index p_index) override;

			void update(const bc_actor_component_manager* p_manager, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			bcSIZE size() override;

			bcSIZE capacity() override;

		protected:

		private:
			core::bc_vector< core::bc_nullable< TComponent > > m_components;
		};

		template< class TComponent >
		bc_actor_component_container<TComponent>::bc_actor_component_container()
		{
			static_assert(std::is_base_of< bc_iactor_component, TComponent >::value, "TComponent must inherit from bc_iactor_component");
		}

		template< class TComponent >
		bc_actor_component_container<TComponent>::bc_actor_component_container(bc_actor_component_container&&) noexcept = default;

		template< class TComponent >
		bc_actor_component_container<TComponent>::~bc_actor_component_container() = default;

		template< class TComponent >
		bc_actor_component_container<TComponent>& bc_actor_component_container<TComponent>::operator=(bc_actor_component_container&&) noexcept = default;

		template< class TComponent >
		bc_iactor_component* bc_actor_component_container<TComponent>::get(bc_actor_component_index p_index)
		{
			auto& l_entry = m_components[p_index];

			if(!l_entry.is_set())
			{
				return nullptr;
			}

			return &l_entry.get();
		}

		template< class TComponent >
		bc_actor_component_index bc_actor_component_container<TComponent>::create()
		{
			bc_actor_component_index l_index = 0;
			bool l_has_setted = false;

			for (core::bc_nullable< TComponent >& l_component : m_components) // TODO use fast free space lookup
			{
				if (!l_component.is_set())
				{
					l_component.reset(TComponent(l_index));
					l_has_setted = true;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_setted)
			{
				m_components.push_back(core::bc_nullable< TComponent >(TComponent(l_index)));
			}

			return l_index;
		}

		template< class TComponent >
		bc_actor_component_index bc_actor_component_container<TComponent>::create_after(bc_actor_component_index p_parent_index)
		{
			bc_actor_component_index l_index = 0;
			bool l_has_setted = false;

			auto l_begin = std::begin(m_components) + p_parent_index + 1;
			auto l_end = std::end(m_components);
			for (; l_begin != l_end; ++l_begin)
			{
				core::bc_nullable< TComponent >& l_component = *l_begin;

				if (!l_component.is_set())
				{
					l_component.reset(TComponent(l_index));
					l_has_setted = true;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_setted)
			{
				m_components.push_back(core::bc_nullable< TComponent >(TComponent(l_index)));
			}

			return l_index;
		}

		template< class TComponent >
		void bc_actor_component_container<TComponent>::remove(bc_actor_component_index p_index)
		{
			m_components[p_index].reset();
		}

		template< class TComponent >
		void bc_actor_component_container<TComponent>::update(const bc_actor_component_manager* p_manager, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			for(auto& l_component : m_components)
			{
				if(l_component.is_set())
				{
					bc_actor l_actor = p_manager->component_get_actor< TComponent >(l_component.get());
					
					l_component->update(l_actor, p_clock_update_param);
				}
			}
		}

		template< class TComponent >
		bcSIZE bc_actor_component_container<TComponent>::size()
		{
			return m_components.size();
		}

		template< class TComponent >
		bcSIZE bc_actor_component_container<TComponent>::capacity()
		{
			return m_components.capacity();
		}
	}
}
