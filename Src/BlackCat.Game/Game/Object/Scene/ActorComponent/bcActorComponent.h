// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Core/bcConstant.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
		class bc_query_manager;
	}
	
	namespace game
	{
		class bc_game_system;
		class bc_actor_component_manager;
		class bc_scene;
		class bc_shape_drawer;
		
		using bc_actor_component_hash = core::bc_string_cmp_hash;
		using bc_actor_component_id = bcINT32;

		struct bc_actor_component_initialize_context
		{
			bc_actor_component_initialize_context(const core::bc_data_driven_parameter& p_parameters,
				const core::bc_data_driven_parameter& p_instance_parameters,
				const core::bc_content_stream_manager& p_stream_manager,
				bc_game_system& p_game_system,
				bc_scene& p_scene,
				bc_actor& p_actor,
				const core::bc_matrix4f& p_transform)
				: m_parameters(p_parameters),
				m_instance_parameters(p_instance_parameters),
				m_stream_manager(p_stream_manager),
				m_game_system(p_game_system),
				m_scene(p_scene),
				m_actor(p_actor),
				m_transform(p_transform)
			{
			}

			const core::bc_data_driven_parameter& m_parameters;
			const core::bc_data_driven_parameter& m_instance_parameters;
			const core::bc_content_stream_manager& m_stream_manager;
			bc_game_system& m_game_system;
			bc_scene& m_scene;
			bc_actor& m_actor;
			const core::bc_matrix4f& m_transform;
		};

		struct bc_actor_component_initialize_entity_context
		{
			bc_actor_component_initialize_entity_context(const core::bc_data_driven_parameter& p_parameters,
				const core::bc_data_driven_parameter& p_instance_parameters,
				const core::bc_content_stream_manager& p_stream_manager,
				bc_game_system& p_game_system,
				bc_scene& p_scene,
				bc_actor& p_actor,
				const core::bc_matrix4f& p_transform)
				: m_parameters(p_parameters),
				m_instance_parameters(p_instance_parameters),
				m_stream_manager(p_stream_manager),
				m_game_system(p_game_system),
				m_scene(p_scene),
				m_actor(p_actor),
				m_transform(p_transform)
			{
			}

			const core::bc_data_driven_parameter& m_parameters;
			const core::bc_data_driven_parameter& m_instance_parameters;
			const core::bc_content_stream_manager& m_stream_manager;
			bc_game_system& m_game_system;
			bc_scene& m_scene;
			bc_actor& m_actor;
			const core::bc_matrix4f& m_transform;
		};

		struct bc_actor_component_load_context
		{
			bc_actor_component_load_context(const core::bc_json_key_value& p_parameters, bc_actor& p_actor)
				: m_parameters(p_parameters),
				m_actor(p_actor)
			{
			}

			const core::bc_json_key_value& m_parameters;
			bc_actor& m_actor;
		};

		struct bc_actor_component_write_context
		{
			bc_actor_component_write_context(core::bc_json_key_value& p_parameters, bc_actor& p_actor)
				: m_parameters(p_parameters),
				m_actor(p_actor)
			{
			}

			core::bc_json_key_value& m_parameters;
			bc_actor& m_actor;
		};

		struct bc_actor_component_network_load_context
		{
			bc_actor_component_network_load_context(const core::bc_json_key_value& p_parameters, bc_actor& p_actor, bool p_is_replication_load, bool p_is_self_replicated)
				: m_parameters(p_parameters),
				m_actor(p_actor),
				m_is_self_replicated(p_is_self_replicated),
				m_is_replication_load(p_is_replication_load)
			{
			}

			const core::bc_json_key_value& m_parameters;
			bc_actor& m_actor;
			bool m_is_self_replicated;
			bool m_is_replication_load;
		};

		struct bc_actor_component_network_write_context
		{
			bc_actor_component_network_write_context(core::bc_json_key_value& p_parameters, bc_actor& p_actor, bool p_is_replication_write)
				: m_parameters(p_parameters),
				m_actor(p_actor),
				m_is_replication_write(p_is_replication_write)
			{
			}

			core::bc_json_key_value& m_parameters;
			bc_actor& m_actor;
			bool m_is_replication_write;
		};

		struct bc_actor_component_update_content
		{
			bc_actor_component_update_content(const core_platform::bc_clock::update_param& p_clock, 
				core::bc_query_manager& p_query_manager, 
				bc_game_system& p_game_system, 
				bc_actor& p_actor, 
				bool p_is_double_update = false)
				: m_clock(p_clock),
				m_query_manager(p_query_manager),
				m_game_system(p_game_system),
				m_actor(p_actor),
				m_is_double_update(p_is_double_update)
			{
			}

			const core_platform::bc_clock::update_param& m_clock;
			core::bc_query_manager& m_query_manager;
			bc_game_system& m_game_system;
			bc_actor& m_actor;
			bool m_is_double_update;
		};

		struct bc_actor_component_event_context
		{
			bc_actor_component_event_context(core::bc_query_manager& p_query_manager, 
				bc_game_system& p_game_system, 
				bc_actor& p_actor, 
				const bc_actor_event& p_event)
				: m_query_manager(p_query_manager),
				m_game_system(p_game_system),
				m_actor(p_actor),
				m_event(p_event)
			{
			}

			core::bc_query_manager& m_query_manager;
			bc_game_system& m_game_system;
			bc_actor& m_actor;
			const bc_actor_event& m_event;
		};

		struct bc_actor_component_debug_draw_context
		{
			bc_actor_component_debug_draw_context(bc_shape_drawer& p_shape_drawer, const bc_actor& p_actor)
				: m_shape_drawer(p_shape_drawer),
				m_actor(p_actor)
			{
			}

			bc_shape_drawer& m_shape_drawer;
			const bc_actor& m_actor;
		};
		
		class BC_GAME_DLL bci_actor_component
		{
		public:
			constexpr static bc_actor_component_id invalid_id = static_cast<bc_actor_component_id>(-1);

		public:
			virtual ~bci_actor_component();

			bc_actor_id get_actor_id() const noexcept;
			
			bc_actor_component_id get_id() const noexcept;
			
			virtual bc_actor get_actor() const noexcept = 0; // TODO provide default implementation

			/**
			 * \brief Must be used to initialize component data members that are shared between instances of this component.
			 * \n In other word data members that act like entity template data.
			 * \param p_context
			 */
			virtual void initialize(const bc_actor_component_initialize_context& p_context);

			/**
			 * \brief Will be called after all components of actor have been added and initialized.
			 * \param p_context 
			 */
			virtual void initialize_entity(const bc_actor_component_initialize_entity_context& p_context);
			
			virtual void load_instance(const bc_actor_component_load_context& p_context);

			virtual void write_instance(const bc_actor_component_write_context& p_context);

			virtual void load_network_instance(const bc_actor_component_network_load_context& p_context);

			virtual void write_network_instance(const bc_actor_component_network_write_context& p_context);

			virtual void update(const bc_actor_component_update_content& p_context);

			virtual void handle_event(const bc_actor_component_event_context& p_context);

			virtual void debug_draw(const bc_actor_component_debug_draw_context& p_context);
			
		protected:
			bci_actor_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bci_actor_component(bci_actor_component&& p_other) noexcept;

			bci_actor_component& operator=(bci_actor_component&& p_other) noexcept;

			static bc_actor_component_manager& get_manager() noexcept;

		private:
			bc_actor_id m_actor_index;
			bc_actor_component_id m_index;
		};

		class BC_GAME_DLL bci_actor_abstract_component : public virtual bci_actor_component
		{
		protected:
			bci_actor_abstract_component() noexcept {}

			bci_actor_abstract_component(bci_actor_abstract_component&&) noexcept = default;

			virtual ~bci_actor_abstract_component() override = default;

			bci_actor_abstract_component& operator=(bci_actor_abstract_component&&) noexcept = default;
		};

		inline bci_actor_component::~bci_actor_component() = default;

		inline bc_actor_id bci_actor_component::get_actor_id() const noexcept
		{
			return m_actor_index;
		}
		
		inline bc_actor_component_id bci_actor_component::get_id() const noexcept
		{
			return m_index;
		}

		inline void bci_actor_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		inline void bci_actor_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
		}
		
		inline void bci_actor_component::load_instance(const bc_actor_component_load_context& p_context)
		{
		}

		inline void bci_actor_component::write_instance(const bc_actor_component_write_context& p_context)
		{
		}

		inline void bci_actor_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
		}

		inline void bci_actor_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
		}
		
		inline void bci_actor_component::update(const bc_actor_component_update_content& p_context)
		{
		}

		inline void bci_actor_component::handle_event(const bc_actor_component_event_context& p_context)
		{
		}
		
		inline void bci_actor_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
		}
		
		inline bci_actor_component::bci_actor_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: m_actor_index(p_actor_index),
			m_index(p_index)
		{
		}

		inline bci_actor_component::bci_actor_component(bci_actor_component&& p_other) noexcept
			: m_actor_index(p_other.m_actor_index),
			m_index(p_other.m_index)
		{
		}

		inline bci_actor_component& bci_actor_component::operator=(bci_actor_component&& p_other) noexcept
		{
			m_actor_index = p_other.m_actor_index;
			m_index = p_other.m_index;

			return *this;
		}
	}
}