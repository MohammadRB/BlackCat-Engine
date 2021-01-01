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
			bc_actor_component_load_context(bc_actor& p_actor,
				const core::bc_json_key_value& p_parameters)
				: m_actor(p_actor),
				m_parameters(p_parameters)
			{
			}

			bc_actor& m_actor;
			const core::bc_json_key_value& m_parameters;
		};

		struct bc_actor_component_write_context
		{
			bc_actor_component_write_context(bc_actor& p_actor,
				core::bc_json_key_value& p_parameters)
				: m_actor(p_actor),
				m_parameters(p_parameters)
			{
			}

			bc_actor& m_actor;
			core::bc_json_key_value& m_parameters;
		};

		struct bc_actor_component_update_content
		{
			bc_actor_component_update_content(bc_actor& p_actor,
				const core_platform::bc_clock::update_param& p_clock): m_actor(p_actor),
				m_clock(p_clock)
			{
			}

			bc_actor& m_actor;
			const core_platform::bc_clock::update_param& m_clock;
		};
		
		class BC_GAME_DLL bc_iactor_component
		{
		public:
			constexpr static bc_actor_component_index invalid_index = static_cast< bc_actor_component_index >(-1);

		public:
			virtual ~bc_iactor_component();

			bc_actor_component_index get_index() const noexcept;
			
			virtual bc_actor get_actor() const noexcept = 0; // TODO provide default implementation

			/**
			 * \brief Initialize component data members that are shared between instances of this component.
			 * In other word data members that act like entity template data.
			 * \param p_context
			 * \param p_parameters
			 * \param p_stream_manager 
			 */
			virtual void initialize(bc_actor_component_initialize_context& p_context);

			virtual void load_instance(bc_actor_component_load_context& p_context);

			virtual void write_instance(bc_actor_component_write_context& p_context);

			virtual void update(bc_actor_component_update_content& p_context);

			virtual void handle_event(bc_actor& p_actor, const bc_actor_event& p_event);

			virtual void debug_draw(const bc_actor& p_actor, bc_shape_drawer& p_shape_drawer);
			
		protected:
			explicit bc_iactor_component(bc_actor_component_index p_index) noexcept;

			bc_iactor_component(bc_iactor_component&& p_other) noexcept;

			bc_iactor_component& operator=(bc_iactor_component&& p_other) noexcept;

			static bc_actor_component_manager& get_manager() noexcept;

		private:
			bc_actor_component_index m_index;
		};

		inline bc_iactor_component::~bc_iactor_component() = default;

		inline bc_actor_component_index bc_iactor_component::get_index() const noexcept
		{
			return m_index;
		}

		inline void bc_iactor_component::initialize(bc_actor_component_initialize_context& p_context)
		{
		}

		inline void bc_iactor_component::load_instance(bc_actor_component_load_context& p_context)
		{
		}

		inline void bc_iactor_component::write_instance(bc_actor_component_write_context& p_context)
		{
		}

		inline void bc_iactor_component::update(bc_actor_component_update_content& p_context)
		{
		}

		inline void bc_iactor_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}
		
		inline void bc_iactor_component::debug_draw(const bc_actor& p_actor, bc_shape_drawer& p_shape_drawer)
		{
		}
		
		inline bc_iactor_component::bc_iactor_component(bc_actor_component_index p_index) noexcept
			: m_index(p_index)
		{
		}

		inline bc_iactor_component::bc_iactor_component(bc_iactor_component&& p_other) noexcept
			: m_index(p_other.m_index)
		{
		}

		inline bc_iactor_component& bc_iactor_component::operator=(bc_iactor_component&& p_other) noexcept
		{
			m_index = p_other.m_index;

			return *this;
		}
	}
}