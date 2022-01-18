// [05/27/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcNetworkReplicateActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRigidDynamicSleepActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_network_component::bc_network_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			m_id(bc_actor::invalid_id),
			m_network_type(bc_network_type::not_started),
			m_data_dir(bc_actor_network_data_dir::replicate),
			m_sync_enabled(true),
			m_network_entity_name(nullptr),
			m_out_ping(0),
			m_in_ping(0)
		{
		}

		bc_network_component::bc_network_component(bc_network_component&&) noexcept = default;

		bc_network_component::~bc_network_component() = default;

		bc_network_component& bc_network_component::operator=(bc_network_component&&) noexcept = default;

		bc_actor bc_network_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_network_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_network_type = p_context.m_game_system.get_network_system().get_network_type();
			const auto& l_data_dir_param = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_network_data_dir);
			const auto* l_network_entity_name_param = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_network_entity_name);
			
			if(l_data_dir_param == "replicate")
			{
				m_data_dir = bc_actor_network_data_dir::replicate;
			}
			else if(l_data_dir_param == "replicate_sync")
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync;
			}
			else if(l_data_dir_param == "replicate_sync_from_client")
			{
				m_data_dir = bc_actor_network_data_dir::replicate_sync_from_client;
			}
			else if(l_data_dir_param == "replicate_sync_to_server_client")
			{
				if(m_network_type == bc_network_type::server)
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

		void bc_network_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			auto* l_rigid_dynamic_component = p_context.m_actor.get_component<bc_rigid_dynamic_component>();
			if(l_rigid_dynamic_component)
			{
				{
					bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

					auto l_rigid_dynamic = l_rigid_dynamic_component->get_dynamic_body();
					l_rigid_dynamic.set_actor_flag(physics::bc_actor_flag::send_sleep_wake, true);
				}
			}
		}

		void bc_network_component::load_network_instance(const bc_actor_component_network_load_context& p_context)
		{
			auto* l_net_id_param = p_context.m_parameters.find("nid")->second.as<bc_actor_network_id>();
			if(!l_net_id_param)
			{
				m_id = bc_actor::invalid_id;
				return;
			}

			m_id = *l_net_id_param;
		}

		void bc_network_component::write_network_instance(const bc_actor_component_network_write_context& p_context)
		{
			p_context.m_parameters.add("nid", core::bc_any(m_id));
		}

		void bc_network_component::update(const bc_actor_component_update_content& p_context)
		{
		}

		void bc_network_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_rigid_dynamic_sleep_event = core::bci_message::as<bc_rigid_dynamic_sleep_actor_event>(p_context.m_event);
			if(l_rigid_dynamic_sleep_event)
			{
				set_sync_enabled(!l_rigid_dynamic_sleep_event->get_is_sleep());
				return;
			}
			
			const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				auto& l_network_system = p_context.m_game_system.get_network_system();
				const auto l_network_type = l_network_system.get_network_type();

				if (l_network_type == bc_network_type::server)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync)
					{
						l_network_system.add_actor_to_sync(p_context.m_actor);
					}
					else if (m_data_dir == bc_actor_network_data_dir::replicate)
					{
						l_network_system.send_message(bc_actor_replicate_network_message(p_context.m_actor));
					}
					else if (m_id == bc_actor::invalid_id && (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client))
					{
						// we should remove network actors if they are not loaded through network, because they will be replicated by the remote host
						l_scene_add_event->get_scene().remove_actor(p_context.m_actor);
					}
					else if (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
					{
						p_context.m_actor.add_event(bc_network_replicate_actor_event(m_data_dir));
					}
				}
				else if (l_network_type == bc_network_type::client)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
					{
						l_network_system.add_actor_to_sync(p_context.m_actor);
					}
					else if (m_id == bc_actor::invalid_id && (m_data_dir == bc_actor_network_data_dir::replicate_sync || m_data_dir == bc_actor_network_data_dir::replicate))
					{
						// we should remove network actors if they are not loaded through network, because they will be replicated by the remote host
						l_scene_add_event->get_scene().remove_actor(p_context.m_actor);
					}
					else if (m_data_dir == bc_actor_network_data_dir::replicate_sync || m_data_dir == bc_actor_network_data_dir::replicate)
					{
						p_context.m_actor.add_event(bc_network_replicate_actor_event(m_data_dir));
					}
				}

				return;
			}

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				auto& l_network_system = p_context.m_game_system.get_network_system();
				const auto l_network_type = l_network_system.get_network_type();

				if (l_network_type == bc_network_type::server)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync)
					{
						l_network_system.remove_actor_from_sync(p_context.m_actor);
					}
					else if(m_id != bc_actor::invalid_id)
					{
						l_network_system.actor_removed(p_context.m_actor);
					}
				}
				else if (l_network_type == bc_network_type::client)
				{
					if (m_data_dir == bc_actor_network_data_dir::replicate_sync_from_client)
					{
						l_network_system.remove_actor_from_sync(p_context.m_actor);
					}
					else if (m_id != bc_actor::invalid_id)
					{
						l_network_system.actor_removed(p_context.m_actor);
					}
				}
				
				return;
			}
		}

		void bc_network_component::add_extrapolating_value(const bcCHAR* p_name, const core::bc_vector3f& p_value)
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_extrapolators),
				std::end(m_extrapolators),
				[=](const auto& p_entry)
				{
					return std::strcmp(std::get<0>(p_entry), p_name) == 0;
				}
			);
			
			if(l_ite != std::end(m_extrapolators))
			{
				std::get<1>(*l_ite).add_sample(p_value);
				std::get<2>(*l_ite) = 0;
			}
			else
			{
				m_extrapolators.push_back(std::make_tuple(p_name, bc_network_v3_extrapolation(p_value), 0));
			}
		}

		std::pair<bool, core::bc_vector3f> bc_network_component::get_extrapolated_value(const bcCHAR* p_name) const noexcept
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_extrapolators),
				std::end(m_extrapolators),
				[=](const auto& p_entry)
				{
					return std::strcmp(std::get<0>(p_entry), p_name) == 0;
				}
			);
			if(l_ite != std::end(m_extrapolators))
			{
				return std::make_pair(true, std::get<1>(*l_ite).change_rate());
			}

			return std::make_pair(false, core::bc_vector3f());
		}

		std::pair<bool, core::bc_vector3f> bc_network_component::get_extrapolated_value(const bcCHAR* p_name, const core_platform::bc_clock::update_param& p_clock) const noexcept
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_extrapolators),
				std::end(m_extrapolators),
				[=](const auto& p_entry)
				{
					return std::strcmp(std::get<0>(p_entry), p_name) == 0;
				}
			);
			if(l_ite == std::end(m_extrapolators))
			{
				return std::make_pair(false, core::bc_vector3f());
			}

			auto& l_extrapolator = std::get<1>(*l_ite);
			auto& l_extrapolator_elapsed = std::get<2>(*l_ite);
			
			const auto l_extrapolation_ratio = p_clock.m_average_elapsed / static_cast<core_platform::bc_clock::small_delta_time>(m_in_ping);
			auto l_extrapolated_value = l_extrapolator.change_rate();
			l_extrapolated_value *= l_extrapolation_ratio;

			l_extrapolator_elapsed += p_clock.m_average_elapsed;
			if(l_extrapolator_elapsed > m_in_ping * 4)
			{
				// If we had no new sample repeat last sample to prevent false extrapolation
				l_extrapolator.add_sample();
				l_extrapolator_elapsed = 0;
			}
			
			/*core::bc_log(core::bc_log_type::debug) << core::bc_only_file
				<< "extrapolation ratio: " << l_extrapolation_ratio
				<< " elapsed: " << p_clock.m_average_elapsed
				<< " ping: " << m_in_ping
				<< core::bc_lend;*/
			return std::make_pair(true, l_extrapolated_value);
		}
	}
}