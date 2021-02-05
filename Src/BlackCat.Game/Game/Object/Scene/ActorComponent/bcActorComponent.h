// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/bcConstant.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}
	
	namespace game
	{
		class bc_actor_component_manager;
		class bc_shape_drawer;
		
		using bc_actor_component_hash = bcSIZE;
		using bc_actor_component_index = bcINT32;

		struct bc_actor_component_initialize_context
		{
			bc_actor_component_initialize_context(bc_actor& p_actor, 
				const core::bc_data_driven_parameter& p_parameters,
				const core::bc_content_stream_manager& p_stream_manager)
				: m_actor(p_actor),
				m_parameters(p_parameters),
				m_stream_manager(p_stream_manager)
			{
			}

			bc_actor& m_actor;
			const core::bc_data_driven_parameter& m_parameters;
			const core::bc_content_stream_manager& m_stream_manager;
		};

		struct bc_actor_component_load_context
		{
			bc_actor_component_load_context(bc_actor& p_actor, const core::bc_json_key_value& p_parameters)
				: m_actor(p_actor),
				m_parameters(p_parameters)
			{
			}

			bc_actor& m_actor;
			const core::bc_json_key_value& m_parameters;
		};

		struct bc_actor_component_write_context
		{
			bc_actor_component_write_context(bc_actor& p_actor, core::bc_json_key_value& p_parameters)
				: m_actor(p_actor),
				m_parameters(p_parameters)
			{
			}

			bc_actor& m_actor;
			core::bc_json_key_value& m_parameters;
		};

		struct bc_actor_component_update_content
		{
			bc_actor_component_update_content(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock): m_actor(p_actor),
				m_clock(p_clock)
			{
			}

			bc_actor& m_actor;
			const core_platform::bc_clock::update_param& m_clock;
		};

		struct bc_actor_component_event_context
		{
			bc_actor_component_event_context(bc_actor& p_actor, const bc_actor_event& p_event)
				: m_actor(p_actor),
				m_event(p_event)
			{
			}

			bc_actor& m_actor;
			const bc_actor_event& m_event;
		};

		struct bc_actor_component_debug_draw_context
		{
			bc_actor_component_debug_draw_context(const bc_actor& p_actor, bc_shape_drawer& p_shape_drawer)
				: m_actor(p_actor),
				m_shape_drawer(p_shape_drawer)
			{
			}

			const bc_actor& m_actor;
			bc_shape_drawer& m_shape_drawer;
		};
		
		class BC_GAME_DLL bci_actor_component
		{
		public:
			constexpr static bc_actor_component_index invalid_index = static_cast< bc_actor_component_index >(-1);

		public:
			virtual ~bci_actor_component();

			bc_actor_index get_actor_index() const noexcept;
			
			bc_actor_component_index get_index() const noexcept;
			
			virtual bc_actor get_actor() const noexcept = 0; // TODO provide default implementation

			/**
			 * \brief Initialize component data members that are shared between instances of this component.
			 * In other word data members that act like entity template data.
			 * \param p_context
			 */
			virtual void initialize(const bc_actor_component_initialize_context& p_context);

			virtual void load_instance(const bc_actor_component_load_context& p_context);

			virtual void write_instance(const bc_actor_component_write_context& p_context);

			virtual void update(const bc_actor_component_update_content& p_context);

			virtual void handle_event(const bc_actor_component_event_context& p_context);

			virtual void debug_draw(const bc_actor_component_debug_draw_context& p_context);
			
		protected:
			bci_actor_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept;

			bci_actor_component(bci_actor_component&& p_other) noexcept;

			bci_actor_component& operator=(bci_actor_component&& p_other) noexcept;

			static bc_actor_component_manager& get_manager() noexcept;

		private:
			bc_actor_index m_actor_index;
			bc_actor_component_index m_index;
		};

		inline bci_actor_component::~bci_actor_component() = default;

		inline bc_actor_index bci_actor_component::get_actor_index() const noexcept
		{
			return m_actor_index;
		}
		
		inline bc_actor_component_index bci_actor_component::get_index() const noexcept
		{
			return m_index;
		}

		inline void bci_actor_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		inline void bci_actor_component::load_instance(const bc_actor_component_load_context& p_context)
		{
		}

		inline void bci_actor_component::write_instance(const bc_actor_component_write_context& p_context)
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
		
		inline bci_actor_component::bci_actor_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept
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