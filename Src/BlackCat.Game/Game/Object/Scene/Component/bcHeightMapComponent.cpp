// [09/14/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "Core/Content/bcContentManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/bcConstant.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_height_map_component::bc_height_map_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_render_component(),
			bc_decal_resolver_component()
		{
		}

		bc_height_map_component::bc_height_map_component(bc_height_map_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_render_component(std::move(p_other)),
			bc_decal_resolver_component(std::move(p_other)),
			m_height_map(std::move(p_other.m_height_map))
		{
		}

		bc_height_map_component::~bc_height_map_component()
		{
		}

		bc_height_map_component& bc_height_map_component::operator=(bc_height_map_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_render_component::operator=(std::move(p_other));
			bc_decal_resolver_component::operator=(std::move(p_other));
			m_height_map = std::move(p_other.m_height_map);

			return *this;
		}

		bc_actor bc_height_map_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_height_map_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_height_map_name = p_context.m_parameters.get_value_throw< core::bc_string >(constant::g_param_heightmap);
			m_height_map = p_context.m_stream_manager.find_content_throw<bc_height_map>(l_height_map_name.c_str());
		}

		void bc_height_map_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			auto* l_content_manager = core::bc_get_service< core::bc_content_manager >();
			l_content_manager->save(*m_height_map);
		}

		void bc_height_map_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as< bc_world_transform_actor_event >(p_context.m_event);
			if(l_world_transform_event)
			{
				m_transform.translate(l_world_transform_event->get_transform().get_translation());
				
				p_context.m_actor.add_event(bc_bound_box_changed_actor_event
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

		void bc_height_map_component::render(const bc_actor_component_render_context& p_context) const
		{
			const bc_render_instance l_instance(m_transform);
			p_context.m_buffer.add_render_instance(m_height_map->get_render_state_ptr(), l_instance);
		}

		void bc_height_map_component::add_decal(const bcCHAR* p_decal_name, const core::bc_vector3f& p_world_position, const core::bc_vector3f& p_dir)
		{
			auto l_actor = get_actor();
			auto* l_decal_component = l_actor.get_component<bc_decal_component>();

			if(!l_decal_component)
			{
				l_actor.create_component<bc_decal_component>();
				l_decal_component = l_actor.get_component<bc_decal_component>();
			}

			const auto l_local_pos = p_world_position - m_transform.get_translation();
			const auto l_world_pos = p_world_position - l_local_pos;
			core::bc_matrix3f l_local_rotation;

			if(graphic::bc_render_api_info::use_left_handed())
			{
				l_local_rotation.rotation_between_two_vector_lh(core::bc_vector3f::up(), p_dir);
			}
			else
			{
				l_local_rotation.rotation_between_two_vector_rh(core::bc_vector3f::up(), p_dir);
			}

			l_decal_component->add_decal
			(
				p_decal_name,
				l_local_pos,
				l_local_rotation,
				core::bc_matrix4f::translation_matrix(l_world_pos),
				bc_mesh_node::s_invalid_index
			);
		}
	}
}