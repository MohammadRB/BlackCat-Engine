// [04/27/2016 MRB]

#pragma once

#include "Core/File/bcJsonDocument.h"
#include "Game/Object/Scene/ActorComponent/bcActorId.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorExtensions.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager_container;
		class bc_entity_manager;
		class bc_shape_drawer;

		class bc_actor
		{
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
			const TComponent* get_component() const noexcept;

			template<class TComponent>
			TComponent* get_create_component() noexcept;

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

			bool is_valid_deep() const noexcept;
			
			bool operator==(const bc_actor& p_other) const noexcept;

			bool operator!=(const bc_actor& p_other) const noexcept;

		private:
			bc_actor_component_manager_container& _get_manager() const noexcept;
			
			bc_entity_manager& _get_entity_manager() const noexcept;

			bc_actor_id m_id;
		};
	}
}
