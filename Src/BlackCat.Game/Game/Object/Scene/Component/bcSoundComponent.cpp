// [03/12/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentStreamManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/System/Sound/bcSoundManager.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_sound_component::bc_sound_component(bc_actor_id p_actor_id, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_id, p_index),
			m_scene(nullptr)
		{
		}

		bc_sound_component::~bc_sound_component() noexcept
		{
			for (auto& l_sound_entry : m_sounds)
			{
				if (l_sound_entry.m_channel.is_valid())
				{
					l_sound_entry.m_channel.stop();
				}
			}
		}

		bc_actor bc_sound_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		bcUINT32 bc_sound_component::get_max_length() const noexcept
		{
			bcUINT32 l_max_length = 0;

			for (const auto& l_sound_entry : m_sounds)
			{
				l_max_length = std::max(l_max_length, l_sound_entry.m_sound->get_resource().get_length());
			}

			return l_max_length;
		}

		sound::bc_sound bc_sound_component::get_sound(core::bc_string_view p_name) const noexcept
		{
			const auto* l_entry = _find_sound(p_name);
			if (!l_entry)
			{
				return sound::bc_sound();
			}

			return l_entry->m_sound->get_resource();
		}

		sound::bc_channel bc_sound_component::play_sound(core::bc_string_view p_name) const noexcept
		{
			auto* l_entry = _find_sound(p_name);
			if (!l_entry || !m_scene)
			{
				return sound::bc_channel();
			}

			auto& l_sound_manager = m_scene->get_sound_manager();
			l_entry->m_channel = l_sound_manager.play_sound(l_entry->m_sound->get_resource(), true);
			l_entry->m_channel.set_mode(l_entry->m_mode);

			if (l_entry->m_min_max_distance.has_value())
			{
				l_entry->m_channel.set_3d_min_max_distance(l_entry->m_min_max_distance->x, l_entry->m_min_max_distance->y);
			}

			if (l_entry->m_volume.has_value())
			{
				l_entry->m_channel.set_volume(*l_entry->m_volume);
			}

			l_entry->m_channel.resume();
			return l_entry->m_channel;
		}

		void bc_sound_component::play_sounds() const noexcept
		{
			for (const auto& l_sound_entry : m_sounds)
			{
				play_sound(l_sound_entry.m_name);
			}
		}

		void bc_sound_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto l_add_icon = false;
			const auto& l_sounds = p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>("sounds");
			m_sounds.reserve(l_sounds.size());

			for (auto& l_entry : l_sounds)
			{
				auto& l_sound_params = l_entry.as_throw<core::bc_json_key_value>();

				core::bc_string l_sound_param;
				core::bc_string l_sound_name_param;
				core::bc_string l_play_mode_param;
				core::bc_vector2f l_min_max_distance_param;
				bcFLOAT l_volume_param;
				bool l_auto_play_param;

				json_parse::bc_load(l_sound_params, constant::g_param_sound, l_sound_param);
				const auto l_has_name_param = json_parse::bc_load(l_sound_params, constant::g_param_sound_name, l_sound_name_param);
				const auto l_has_play_mode_param = json_parse::bc_load(l_sound_params, constant::g_param_sound_play_mode, l_play_mode_param);
				const auto l_has_min_max_param = json_parse::bc_load(l_sound_params, constant::g_param_sound_min_max_distance, l_min_max_distance_param);
				const auto l_has_volume_param = json_parse::bc_load(l_sound_params, constant::g_param_sound_volume, l_volume_param);
				const auto l_has_auto_play_param = json_parse::bc_load(l_sound_params, constant::g_param_sound_auto_play, l_auto_play_param);

				auto l_sound_mode = sound::bc_sound_mode::none;

				if (l_has_play_mode_param)
				{
					if (l_play_mode_param == "once")
					{
						l_sound_mode = core::bc_enum::set(l_sound_mode, sound::bc_sound_mode::loop_off, true);
					}
					else if (l_play_mode_param == "loop")
					{
						l_sound_mode = core::bc_enum::set(l_sound_mode, sound::bc_sound_mode::loop, true);
						l_add_icon = true;
					}
					else
					{
						throw bc_invalid_argument_exception("invalid sound play mode value");
					}
				}

				auto l_sound = p_context.m_stream_manager.find_content_throw<sound::bc_sound_content>(l_sound_param);
				_bc_sound_component_sound l_new_sound
				{
					std::move(l_sound_name_param),
					l_sound_mode,
					l_has_min_max_param ? core::bc_nullable<core::bc_vector2f>(l_min_max_distance_param) : core::bc_nullable<core::bc_vector2f>(),
					l_has_volume_param ? core::bc_nullable<bcFLOAT>(l_volume_param) : core::bc_nullable<bcFLOAT>(),
					l_has_auto_play_param ? l_auto_play_param : false,
					std::move(l_sound),
					sound::bc_channel()
				};

				m_sounds.push_back(std::move(l_new_sound));
			}

			if (l_add_icon)
			{
				auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
				l_icon_component->set_icon(bc_icon_type::sound);
			}
		}

		void bc_sound_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
			{
				for (auto& l_sound_entry : m_sounds)
				{
					auto& l_sound_channel = l_sound_entry.m_channel;

					if (l_sound_channel.is_valid() && core::bc_enum::has(l_sound_channel.get_mode(), sound::bc_sound_mode::d3))
					{
						l_sound_channel.set_3d_position(l_world_transform_event->get_transform().get_translation());
						return;
					}
				}
			}

			if (const auto l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event))
			{
				m_scene = &l_added_to_scene_event->get_scene();

				for (auto& l_sound_entry : m_sounds)
				{
					if (l_sound_entry.m_auto_play)
					{
						play_sound(l_sound_entry.m_name);
					}
				}
				return;
			}

			if (core::bci_message::is<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				for (auto& l_sound_entry : m_sounds)
				{
					if (l_sound_entry.m_channel.is_valid())
					{
						l_sound_entry.m_channel.stop();
					}
				}

				m_scene = nullptr;
				return;
			}
		}

		_bc_sound_component_sound* bc_sound_component::_find_sound(core::bc_string_view p_name) const
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_sounds),
				std::end(m_sounds),
				[&](auto& p_entry)
				{
					return p_entry.m_name == p_name;
				}
			);

			if (l_ite == std::end(m_sounds))
			{
				return nullptr;
			}

			return &*l_ite;
		}
	}
}
