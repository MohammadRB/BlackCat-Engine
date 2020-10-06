// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;
		using bc_actor_index = bcINT32;

		class bc_actor
		{
		public:
			constexpr static bc_actor_index invalid_index = bc_actor_index(-1);

		private:
			friend class bc_actor_component_manager;

		public:
			bc_actor();

			explicit bc_actor(bc_actor_index p_index);

			bc_actor(const bc_actor&) noexcept;

			~bc_actor();

			bc_actor& operator=(const bc_actor&) noexcept;

			bc_actor_index get_index() const noexcept;

			template<typename TEvent>
			void add_event(TEvent&& p_event);

			const bc_actor_event* get_events() const;

			template< class TComponent >
			void create_component();

			template< class TComponent >
			void remove_component();

			template< class TComponent >
			bool has_component() const;

			template< class TComponent >
			TComponent* get_component();

			template< class TComponent >
			const TComponent* get_component() const;

			template< class TIterator >
			void get_components(TIterator p_destination) const;

			void destroy();

			bool operator==(const bc_actor& p_other) const noexcept;

			bool operator!=(const bc_actor& p_other) const noexcept;

		private:
			static bc_actor_component_manager& _get_manager() noexcept;

			bc_actor_index m_index;
		};		
	}
}