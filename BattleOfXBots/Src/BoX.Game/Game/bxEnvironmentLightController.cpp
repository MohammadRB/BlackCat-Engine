// [24/04/2022 MRB]

#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "App/bcException.h"
#include "BoX.Game/Game/bxEnvironmentLightController.h"

namespace box
{
	bool bc_is_time_between(bcFLOAT p_start_time, bcFLOAT p_end_time, bcFLOAT p_time)
	{
		if(p_end_time < p_start_time)
		{
			p_end_time += 24;

			if(p_time < 12)
			{
				p_time += 24;
			}
		}

		return p_time >= p_start_time && p_time < p_end_time;
	}

	bcFLOAT bc_normalize_time(bcFLOAT p_start_time, bcFLOAT p_end_time, bcFLOAT p_time)
	{
		if (p_end_time < p_start_time)
		{
			p_end_time += 24;

			if (p_time < 12)
			{
				p_time += 24;
			}
		}

		return (p_time - p_start_time) / (p_end_time - p_start_time);
	}

	template<typename T>
	T bc_lerp(const T& p_first, const T& p_second, bcFLOAT p_time)
	{
		return p_first + (p_second - p_first) * p_time;
	}

	void bx_environment_light_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		m_light_component = p_context.m_actor.get_component<game::bc_light_component>();
		if(!m_light_component || m_light_component->get_light()->as_direct_light() == nullptr)
		{
			throw bc_invalid_operation_exception("environment light controller needs light component with direct light.");
		}

		m_time_change_per_seconds = 0.05f;
		m_sunrise_time = 6.0f;
		m_daylight_time = 13.0f;
		m_sunset_time = 18.0f;
		m_nightlight_time = 21.0f;

		m_sunrise_direction = { 1.f, -.5f, 0.f };
		m_sunrise_color = { .95f, .5f, .4f, 1.f };
		m_sunrise_ambient_color = { 0.66f, 0.68f, 0.70f, 0.5f };

		m_daylight_direction = { 0.f, -.7f, -.7f };
		m_daylight_color = { 0.79f, 0.78f, 0.68f, 1.1f };
		m_daylight_ambient_color = { 0.66f, 0.68f, 0.70f, 0.8f };

		m_sunset_direction = { -1.f, -.5f, 0.f };
		m_sunset_color = { .94f, .58f, .1f, 1.f };
		m_sunset_ambient_color = { 0.66f, 0.68f, 0.70f, 0.5f };

		m_nightlight_direction = { 0.f, -.7f, .7f };
		m_nightlight_color = { .62f, .89f, .92f, 0.6f };
		m_nightlight_ambient_color = { 0.66f, 0.68f, 0.70f, 0.3f };

		json_parse::bc_load(p_context.m_parameters, "time_change_per_second", m_time_change_per_seconds);
		json_parse::bc_load(p_context.m_parameters, "sunrise_time", m_sunrise_time);
		json_parse::bc_load(p_context.m_parameters, "daylight_time", m_daylight_time);
		json_parse::bc_load(p_context.m_parameters, "sunset_time", m_sunset_time);
		json_parse::bc_load(p_context.m_parameters, "nightlight_time", m_nightlight_time);

		json_parse::bc_load(p_context.m_parameters, "sunrise_direction", m_sunrise_direction);
		json_parse::bc_load(p_context.m_parameters, "sunrise_color", m_sunrise_color);
		json_parse::bc_load(p_context.m_parameters, "sunrise_ambient_color", m_sunrise_ambient_color);

		json_parse::bc_load(p_context.m_parameters, "daylight_direction", m_daylight_direction);
		json_parse::bc_load(p_context.m_parameters, "daylight_color", m_daylight_color);
		json_parse::bc_load(p_context.m_parameters, "daylight_ambient_color", m_daylight_ambient_color);

		json_parse::bc_load(p_context.m_parameters, "nightlight_direction", m_nightlight_direction);
		json_parse::bc_load(p_context.m_parameters, "nightlight_color", m_nightlight_color);
		json_parse::bc_load(p_context.m_parameters, "nightlight_ambient_color", m_nightlight_ambient_color);
		
		m_sunrise_direction = core::bc_vector3f::normalize(m_sunrise_direction);
		m_daylight_direction = core::bc_vector3f::normalize(m_daylight_direction);
		m_sunset_direction = core::bc_vector3f::normalize(m_sunset_direction);
		m_nightlight_direction = core::bc_vector3f::normalize(m_nightlight_direction);

		m_current_time = m_sunrise_time;
		m_current_light_direction = m_sunrise_direction;
		m_current_light_color = m_sunrise_color;
		m_current_ambient_color = m_sunrise_ambient_color;
	}

	void bx_environment_light_controller::update(const game::bc_actor_component_update_content& p_context)
	{
		return;

		m_current_time += m_time_change_per_seconds * p_context.m_clock.m_elapsed_second;
		m_current_time = std::fmodf(m_current_time, 24.f);

		const auto l_time_between_sunrise_daylight = bc_is_time_between(m_sunrise_time, m_daylight_time, m_current_time);
		const auto l_time_between_daylight_sunset = bc_is_time_between(m_daylight_time, m_sunset_time, m_current_time);
		const auto l_time_between_sunset_nightlight = bc_is_time_between(m_sunset_time, m_nightlight_time, m_current_time);
		const auto l_time_between_nightlight_sunrise = bc_is_time_between(m_nightlight_time, m_sunrise_time, m_current_time);

		if(l_time_between_sunrise_daylight)
		{
			const auto l_normalized_time = bc_normalize_time(m_sunrise_time, m_daylight_time, m_current_time);
			m_current_light_direction = bc_lerp(m_sunrise_direction, m_daylight_direction, l_normalized_time);
			m_current_light_color = bc_lerp(m_sunrise_color, m_daylight_color, l_normalized_time);
			m_current_ambient_color = bc_lerp(m_sunrise_ambient_color, m_daylight_ambient_color, l_normalized_time);
		}
		else if(l_time_between_daylight_sunset)
		{
			const auto l_normalized_time = bc_normalize_time(m_daylight_time, m_sunset_time, m_current_time);
			m_current_light_direction = bc_lerp(m_daylight_direction, m_sunset_direction, l_normalized_time);
			m_current_light_color = bc_lerp(m_daylight_color, m_sunset_color, l_normalized_time);
			m_current_ambient_color = bc_lerp(m_daylight_ambient_color, m_sunset_ambient_color, l_normalized_time);
		}
		else if(l_time_between_sunset_nightlight)
		{
			const auto l_normalized_time = bc_normalize_time(m_sunset_time, m_nightlight_time, m_current_time);
			m_current_light_direction = bc_lerp(m_sunset_direction, m_nightlight_direction, l_normalized_time);
			m_current_light_color = bc_lerp(m_sunset_color, m_nightlight_color, l_normalized_time);
			m_current_ambient_color = bc_lerp(m_sunset_ambient_color, m_nightlight_ambient_color, l_normalized_time);
		}
		else if(l_time_between_nightlight_sunrise)
		{
			const auto l_normalized_time = bc_normalize_time(m_nightlight_time, m_sunrise_time, m_current_time);
			m_current_light_direction = bc_lerp(m_nightlight_direction, m_sunrise_direction, l_normalized_time);
			m_current_light_color = bc_lerp(m_nightlight_color, m_sunrise_color, l_normalized_time);
			m_current_ambient_color = bc_lerp(m_nightlight_ambient_color, m_sunrise_ambient_color, l_normalized_time);
		}

		auto& l_light = *m_light_component->get_light()->as_direct_light();
		l_light.set_direction(m_current_light_direction);
		l_light.set_color(m_current_light_color.xyz());
		l_light.set_ambient_color(m_current_ambient_color.xyz());
		l_light.set_intensity(m_current_light_color.w);
		l_light.set_ambient_intensity(m_current_ambient_color.w);
	}
}
