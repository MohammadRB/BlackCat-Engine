// [09/14/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "Core/Content/bcContentManager.h"
#include "Game/bcConstant.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"

namespace black_cat
{
	namespace game
	{
		bc_height_map_component::bc_height_map_component(bc_actor_component_index p_index)
			: bc_render_component(p_index)
		{
		}

		bc_height_map_component::bc_height_map_component(bc_height_map_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
			m_height_map(std::move(p_other.m_height_map))
		{
		}

		bc_height_map_component::~bc_height_map_component()
		{
		}

		bc_height_map_component& bc_height_map_component::operator=(bc_height_map_component&& p_other) noexcept
		{
			m_height_map = std::move(p_other.m_height_map);
			bc_render_component::operator=(std::move(p_other));

			return *this;
		}

		bc_actor bc_height_map_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_height_map_component::initialize(bc_actor_component_initialize_context& p_context)
		{
			auto& l_height_map_name = p_context.m_parameters.get_value_throw< core::bc_string >(constant::g_param_heightmap);
			m_height_map = p_context.m_stream_manager.find_content_throw(l_height_map_name.c_str());
		}

		void bc_height_map_component::write_instance(bc_actor_component_write_context& p_context)
		{
			auto* l_content_manager = core::bc_get_service< core::bc_content_manager >();
			l_content_manager->save(*m_height_map);
		}

		void bc_height_map_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			auto* l_world_transform_event = core::bc_imessage::as< bc_actor_event_world_transform >(p_event);
			if(l_world_transform_event)
			{
				m_transform.translate(l_world_transform_event->get_transform().get_translation());
				
				p_actor.add_event(bc_actor_event_bound_box_changed
				(
					physics::bc_bound_box
					(
						m_transform.get_translation(),
						core::bc_vector3f
						(
							(m_height_map->get_width() * m_height_map->get_xz_multiplier()) / 2,
							1024.0 / 2, // TODO Get actual terrain height
							(m_height_map->get_height() * m_height_map->get_xz_multiplier()) / 2
						)
					)
				));
			}
		}

		void bc_height_map_component::render(bc_render_state_buffer& p_buffer) const
		{
			const bc_render_instance l_instance(m_transform);
			p_buffer.add_render_instance(m_height_map->get_render_state_ptr(), l_instance);
		}
	}
}