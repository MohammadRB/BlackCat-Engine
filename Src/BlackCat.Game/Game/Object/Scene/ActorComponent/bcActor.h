// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorExtensions.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;
		class bc_entity_manager;
		class bc_shape_drawer;

		using bc_actor_id = bcINT32;
		using bc_actor_network_id = bcINT32;

		class bc_actor
		{
		public:
			constexpr static bc_actor_id invalid_id = static_cast<bc_actor_id>(-1);

		private:
			friend class bc_actor_component_manager;

		public:
			bc_actor();

			explicit bc_actor(bc_actor_id p_index);

			bc_actor(const bc_actor&) noexcept;
			
			bc_actor(bc_actor&&) noexcept;

			~bc_actor();

			bc_actor& operator=(const bc_actor&) noexcept;
			
			bc_actor& operator=(bc_actor&&) noexcept;

			bc_actor_id get_id() const noexcept;

			const bc_actor_event* get_events() const noexcept;
			
			template<typename TEvent>
			void add_event(TEvent&& p_event);

			template<class TComponent>
			TComponent* get_component() noexcept;

			template<class TComponent>
			TComponent* get_create_component() noexcept;

			template<class TComponent>
			const TComponent* get_component() const noexcept;

			template<class TIterator>
			void get_components(TIterator p_destination) const;

			template<class TComponent>
			bool has_component() const noexcept;

			template<class TComponent>
			void create_component();

			template<class TComponent>
			void create_component(const core::bc_json_key_value& p_instance_parameters);

			template<class TComponent>
			void remove_component();

			void mark_for_double_update();
			
			void draw_debug(bc_shape_drawer& p_shape_drawer) const;
			
			void destroy();

			bool is_valid() const noexcept;
			
			bool operator==(const bc_actor& p_other) const noexcept;

			bool operator!=(const bc_actor& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

		private:
			static bc_actor_component_manager& _get_manager() noexcept;
			
			static bc_entity_manager& _get_entity_manager() noexcept;

			bc_actor_id m_index;
		};
	}
}
