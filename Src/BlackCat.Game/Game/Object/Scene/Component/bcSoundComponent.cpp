// [03/12/2022 MRB]

#include "Game/GamePCH.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcConstant.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_sound_component::bc_sound_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_actor bc_sound_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_sound_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_sound_param = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_sound);
			const auto* l_play_mode_param = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_sound_play_mode);
			const auto l_min_max_distance_param = p_context.m_parameters.get_value_vector2f(constant::g_param_sound_min_max_distance);
			const auto* l_volume_param = p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_sound_volume);
			auto l_sound_mode = sound::bc_sound_mode::none;

			if(l_play_mode_param)
			{
				if(*l_play_mode_param == "once")
				{
					l_sound_mode = core::bc_enum::set(l_sound_mode, sound::bc_sound_mode::loop_off, true);
				}
				else if(*l_play_mode_param == "loop")
				{
					l_sound_mode = core::bc_enum::set(l_sound_mode, sound::bc_sound_mode::loop, true);
				}
				else
				{
					throw bc_invalid_argument_exception("invalid sound play mode value");
				}
			}

			if(l_min_max_distance_param.has_value())
			{
				l_sound_mode = core::bc_enum::set(l_sound_mode, sound::bc_sound_mode::d3, true);
			}

			auto& l_sound_device = p_context.m_game_system.get_sound_system().get_device();
			const auto l_sound_content = p_context.m_stream_manager.find_content_throw<sound::bc_sound_content>(l_sound_param);

			m_sound = l_sound_device.play_sound(l_sound_content->get_resource(), true);
			m_sound.set_mode(l_sound_mode);

			if(l_min_max_distance_param.has_value())
			{
				m_sound.set_3d_min_max_distance(l_min_max_distance_param->x, l_min_max_distance_param->y);
			}

			if(l_volume_param)
			{
				m_sound.set_volume(*l_volume_param);
			}

			if(core::bc_enum::has(l_sound_mode, sound::bc_sound_mode::loop))
			{
				auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
				l_icon_component->set_icon("sound");
			}
		}

		void bc_sound_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if(const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
			{
				if(core::bc_enum::has(m_sound.get_mode(), sound::bc_sound_mode::d3))
				{
					m_sound.set_3d_position(l_world_transform_event->get_transform().get_translation());
					return;
				}
			}

			if(core::bci_message::is<bc_added_to_scene_actor_event>(p_context.m_event))
			{
				m_sound.play();
				return;
			}

			if (core::bci_message::is<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				m_sound.stop();
				return;
			}
		}
	}
}
