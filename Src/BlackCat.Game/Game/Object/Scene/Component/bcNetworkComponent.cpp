// [27/05/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcNetworkReplicateActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRigidDynamicSleepActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_network_component::bc_network_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
		: bci_actor_component(p_actor_id, p_id),
		  m_network_system(nullptr),
		  m_network_entity_name(nullptr),
		  m_data_dir(bc_actor_network_data_dir::replicate),
		  m_replication_side(bc_actor_replication_side::origin),
		  m_network_client_id(bc_network_client::invalid_id),
		  m_network_id(g_invalid_actor_network_id),
		  m_sync_enabled(true),
		  m_out_ping(0),
		  m_in_ping(0)
	{
	}

	bc_network_component::bc_network_component(bc_network_component&& p_other) noexcept
		: bci_actor_component(std::move(p_other)),
		  m_network_system(p_other.m_network_system),
		  m_network_entity_name(p_other.m_network_entity_name),
		  m_data_dir(p_other.m_data_dir),
		  m_replication_side(p_other.m_replication_side),
		  m_network_client_id(p_other.m_network_client_id),
		  m_network_id(p_other.m_network_id),
		  m_sync_enabled(p_other.m_sync_enabled),
		  m_out_ping(p_other.m_out_ping),
		  m_in_ping(p_other.m_in_ping),
		  m_extrapolators(std::move(p_other.m_extrapolators))
	{
		p_other.set_as_invalid_network_state();
	}
		
	bc_network_component& bc_network_component::operator=(bc_network_component&& p_other) noexcept
	{
		bci_actor_component::operator=(std::move(p_other));
		m_network_system = p_other.m_network_system;
		m_network_entity_name = p_other.m_network_entity_name;
		m_data_dir = p_other.m_data_dir;
		m_replication_side = p_other.m_replication_side;
		m_network_client_id = p_other.m_network_client_id;
		m_network_id = p_other.m_network_id;
		m_sync_enabled = p_other.m_sync_enabled;
		m_out_ping = p_other.m_out_ping;
		m_in_ping = p_other.m_in_ping;
		m_extrapolators = std::move(p_other.m_extrapolators);

		p_other.set_as_invalid_network_state();
		return *this;
	}
	
	bc_network_type bc_network_component::get_network_type() const noexcept
	{
		return m_network_system->get_network_type();
	}

	void bc_network_component::initialize(const bc_actor_component_initialize_context& p_context)
	{
		m_network_system = &p_context.m_game_system.get_network_system();

		const auto* l_data_dir_param = static_cast<core::bc_string*>(nullptr);
		const auto* l_network_entity_name_param = static_cast<core::bc_string*>(nullptr);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_network_data_dir, l_data_dir_param);
		json_parse::bc_load(p_context.m_parameters, constant::g_param_network_entity_name, l_network_entity_name_param);
			
		if(*l_data_dir_param == "replicate")
		{
			m_data_dir = bc_actor_network_data_dir::replicate;
		}
		else if(*l_data_dir_param == "replicate_sync")
		{
			m_data_dir = bc_actor_network_data_dir::replicate_sync;
		}
		else if(*l_data_dir_param == "replicate_sync_from_client")
		{
			m_data_dir = bc_actor_network_data_dir::replicate_sync_from_client;
		}
		else if(*l_data_dir_param == "replicate_sync_from_client_server")
		{
			if(get_network_type() == bc_network_type::server)
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync_from_client;
			}
			else
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync;
			}
		}
		else
		{
			throw bc_invalid_argument_exception("Invalid value for network data direction parameter");
		}

		if(get_network_type() != bc_network_type::not_started)
		{
			if
			(
				(get_network_type() == bc_network_type::server && m_data_dir == bc_actor_network_data_dir::replicate_sync) ||
				(get_network_type() == bc_network_type::client && m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
			)
			{
				m_replication_side = bc_actor_replication_side::origin;
			}
			else
			{
				m_replication_side = bc_actor_replication_side::replicated;
			}
		}

		if(l_network_entity_name_param)
		{
			m_network_entity_name = l_network_entity_name_param->c_str();
		}
		else
		{
			const auto* l_mediate_component = p_context.m_actor.get_component<bc_mediate_component>();
			m_network_entity_name = l_mediate_component->get_entity_name();
		}
	}
		
	void bc_network_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
	{
		const auto* l_client_id_param = p_context.m_parameters.find("cid")->second.as<bc_network_client_id>();
		const auto* l_net_id_param = p_context.m_parameters.find("nid")->second.as<bc_actor_network_id>();

		if (l_client_id_param)
		{
			m_network_client_id = *l_client_id_param;
		}
		else
		{
			m_network_client_id = bc_network_client::invalid_id;
		}

		if (l_net_id_param)
		{
			m_network_id = *l_net_id_param;
		}
		else
		{
			m_network_id = g_invalid_actor_network_id;
		}
	}

	void bc_network_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
	{
		p_context.m_parameters.add_or_update("cid", core::bc_any(m_network_client_id));
		p_context.m_parameters.add_or_update("nid", core::bc_any(m_network_id));
	}

	void bc_network_component::update(const bc_actor_component_update_content& p_context)
	{
	}

	void bc_network_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		if(const auto* l_rigid_dynamic_sleep_event = core::bci_message::as<bc_rigid_dynamic_sleep_actor_event>(p_context.m_event))
		{
			set_sync_enabled(!l_rigid_dynamic_sleep_event->get_is_sleep());
			return;
		}

		if (core::bci_message::is<bc_added_to_scene_actor_event>(p_context.m_event))
		{
			_added_to_scene(p_context);
			return;
		}

		if (core::bci_message::is<bc_removed_from_scene_actor_event>(p_context.m_event))
		{
			_removed_from_scene(p_context);
			return;
		}
	}

	void bc_network_component::add_extrapolating_value(core::bc_string_view p_name, const core::bc_vector3f& p_extrapolated_value, const core::bc_vector3f& p_network_value)
	{
		const auto l_ite = std::find_if
		(
			std::begin(m_extrapolators),
			std::end(m_extrapolators),
			[=](const auto& p_entry)
			{
				return p_entry.m_name == p_name;
			}
		);
			
		if(l_ite != std::end(m_extrapolators))
		{
			l_ite->m_extrapolator.add_sample(p_network_value);
			l_ite->m_correction_vector = p_network_value - p_extrapolated_value;
			l_ite->m_elapsed_since_last_update = 0;
		}
		else
		{
			m_extrapolators.push_back(_bc_network_extrapolation_value
				{
					p_name,
					bc_network_v3_extrapolation(p_network_value),
					core::bc_vector3f(0),
					0
				});
		}
	}
		
	std::pair<bool, core::bc_vector3f> bc_network_component::get_extrapolated_value(core::bc_string_view p_name, const platform::bc_clock::update_param& p_clock) const noexcept
	{
		const auto l_ite = std::find_if
		(
			std::begin(m_extrapolators),
			std::end(m_extrapolators),
			[=](const auto& p_entry)
			{
				return p_entry.m_name == p_name;
			}
		);
		if(l_ite == std::end(m_extrapolators))
		{
			return std::make_pair(false, core::bc_vector3f());
		}
			
		l_ite->m_elapsed_since_last_update += p_clock.m_average_elapsed;

		if (m_in_ping >= p_clock.m_average_elapsed && l_ite->m_elapsed_since_last_update > m_in_ping)
		{
			// If we had no new sample, return 0 to prevent false extrapolation
			return { true, {0,0,0} };
		}

		const auto l_time_ratio = static_cast<platform::bc_clock::small_time>(m_in_ping) / p_clock.m_average_elapsed;
		const auto l_correction_value = l_ite->m_correction_vector / l_time_ratio;
		const auto l_extrapolated_value = l_ite->m_extrapolator.change_rate() / l_time_ratio;
			
		return std::make_pair(true, l_extrapolated_value + l_correction_value);
	}

	void bc_network_component::_added_to_scene(const bc_actor_component_event_context& p_context)
	{
		auto& l_network_system = p_context.m_game_system.get_network_system();
		const auto l_network_type = l_network_system.get_network_type();
		const auto& l_scene_add_event = static_cast<const bc_added_to_scene_actor_event&>(p_context.m_event);
		bool l_send_replicate_event = true;

		if (l_network_type == bc_network_type::server)
		{
			switch (m_data_dir)
			{
			case bc_actor_network_data_dir::replicate_sync:
			{
				l_network_system.add_actor_to_sync(p_context.m_actor);
				break;
			}
			case bc_actor_network_data_dir::replicate:
			{
				l_network_system.send_message(bc_actor_replicate_network_message(p_context.m_actor));
				break;
			}
			case bc_actor_network_data_dir::replicate_sync_from_client:
			{
				if (m_network_id == g_invalid_actor_network_id)
				{
					// we should remove network actors if they are not loaded through network because they will be replicated by the remote host
					l_scene_add_event.get_scene().remove_actor(p_context.m_actor);
					l_send_replicate_event = false;

					if (auto* l_rigid_dynamic_component = p_context.m_actor.get_component<bc_rigid_dynamic_component>())
					{
						{
							bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

							// disable physics simulation until actor get removed from scene to prevent its interference
							// with actor which will be replicated by remote part
							l_rigid_dynamic_component->set_enable(false);
						}
					}
				}
				break;
			}
			case bc_actor_network_data_dir::replicate_sync_from_client_server:
				break;
			default:
				BC_ASSERT(false);
			}
		}
		else if (l_network_type == bc_network_type::client)
		{
			switch (m_data_dir)
			{
			case bc_actor_network_data_dir::replicate_sync_from_client:
			{
				l_network_system.add_actor_to_sync(p_context.m_actor);
				break;
			}
			case bc_actor_network_data_dir::replicate:
			case bc_actor_network_data_dir::replicate_sync:
			{
				if (m_network_id == g_invalid_actor_network_id)
				{
					// we should remove network actors if they are not loaded through network because they will be replicated by the remote host
					l_scene_add_event.get_scene().remove_actor(p_context.m_actor);
					l_send_replicate_event = false;

					if (auto* l_rigid_dynamic_component = p_context.m_actor.get_component<bc_rigid_dynamic_component>())
					{
						{
							bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

							// disable physics simulation until actor get removed from scene to prevent its interference
							// with actor which will be replicated by remote part
							l_rigid_dynamic_component->set_enable(false);
						}
					}
				}
				break;
			}
			case bc_actor_network_data_dir::replicate_sync_from_client_server:
				break;
			default:
				BC_ASSERT(false);
			}
		}

		if (l_send_replicate_event)
		{
			p_context.m_actor.add_event(bc_network_replicate_actor_event(m_data_dir, m_replication_side));
		}
	}

	void bc_network_component::_removed_from_scene(const bc_actor_component_event_context& p_context)
	{
		if (m_network_id == g_invalid_actor_network_id)
		{
			return;
		}

		const auto l_network_type = get_network_type();

		if (l_network_type == bc_network_type::server)
		{
			if (m_data_dir == bc_actor_network_data_dir::replicate_sync)
			{
				m_network_system->remove_actor_from_sync(p_context.m_actor);
			}
			else if (m_network_id != g_invalid_actor_network_id)
			{
				m_network_system->actor_removed(p_context.m_actor);
			}
		}
		else if (l_network_type == bc_network_type::client)
		{
			if (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
			{
				m_network_system->remove_actor_from_sync(p_context.m_actor);
			}
			else if (m_network_id != g_invalid_actor_network_id)
			{
				m_network_system->actor_removed(p_context.m_actor);
			}
		}

		set_as_invalid_network_state();
	}
}
