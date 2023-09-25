// [22/02/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "Game/System/Input/bcGlobalConfig.h"

namespace black_cat
{
	game::bc_global_config* g_global_config = nullptr;
}

namespace black_cat::game
{
	bc_global_config::bc_global_config(const bcECHAR* p_content_path, const bcECHAR* p_config_file_name)
		: bc_config_file()
	{
		constexpr core::bc_estring_view l_config_file_extension = bcL(".json");
		core::bc_estring_frame l_config_file_name(p_config_file_name);

		if(!core::bc_string_ends_with(l_config_file_name, l_config_file_extension))
		{
			l_config_file_name.append(l_config_file_extension);
		}

		const auto l_path = core::bc_path(p_content_path).combine(core::bc_path(l_config_file_name));
		load(l_path);

		auto& l_json = *m_json;
		if (!l_json->m_preferred_gpu.has_value())
		{
			*l_json->m_preferred_gpu = "";
		}
		if (!l_json->m_log_types.has_value())
		{
			l_json->m_log_types.new_entry().set("info");
			l_json->m_log_types.new_entry().set("debug");
			l_json->m_log_types.new_entry().set("warning");
			l_json->m_log_types.new_entry().set("error");
		}
		if (!l_json->m_scene_global_scale.has_value())
		{
			*l_json->m_scene_global_scale = 1.f;
		}
		if(!l_json->m_scene_global_view_distance_scale.has_value())
		{
			*l_json->m_scene_global_view_distance_scale = 1.f;
		}
		if (!l_json->m_scene_graph_debug_draw.has_value())
		{
			*l_json->m_scene_graph_debug_draw = false;
		}
		if(!l_json->m_scene_graph_actors_pool_capacity.has_value())
		{
			*l_json->m_scene_graph_actors_pool_capacity = 2000;
		}
		if (!l_json->m_scene_bullet_reference_mass.has_value())
		{
			*l_json->m_scene_bullet_reference_mass = .1f;
		}
		if (!l_json->m_network_client_name.has_value())
		{
			*l_json->m_network_client_name = "";
		}
		if(!l_json->m_counter_values.has_value())
		{
			l_json->m_counter_values.new_entry().set("fps");
			l_json->m_counter_values.new_entry().set("ping");
			l_json->m_counter_values.new_entry().set("frame_time");
			l_json->m_counter_values.new_entry().set("update_time");
			l_json->m_counter_values.new_entry().set("render_time");
			l_json->m_counter_values.new_entry().set("swap_time");
		}
			
		auto l_read_value = [=](const bcCHAR* p_name, auto& p_value)
		{
			bool l_succeeded;
			core::bc_any l_any;
			read_config_key(p_name, l_any, l_succeeded);

			if (l_succeeded)
			{
				auto* l_value = l_any.as<std::remove_reference_t<decltype(p_value)>>();
				if (l_value)
				{
					p_value = *l_value;
				}
			}
			else
			{
				l_any.set_value(p_value);
				add_or_update_config_key(p_name, l_any);
			}
		};

		/*l_read_value("lod_global_scale", m_lod_global_scale);
		l_read_value("lod_culling_index", m_lod_culling_index);*/

		flush_changes();
	}

	core::bc_vector<core::bc_string> bc_global_config::get_log_types() const noexcept
	{
		core::bc_vector<core::bc_string> l_output;
		l_output.reserve((*m_json)->m_log_types.size());

		std::transform
		(
			std::begin((*m_json)->m_log_types),
			std::end((*m_json)->m_log_types),
			std::back_inserter(l_output),
			[](const auto& l_json_value)
			{
				return *l_json_value;
			}
		);

		return l_output;
	}

	core::bc_nullable<bc_render_pass_config> bc_global_config::get_render_pass_config(core::bc_string_view p_name) const noexcept
	{
		auto& l_json = *m_json;

		for (auto& l_pass : l_json->m_render_passes)
		{
			if (*l_pass->m_name == p_name)
			{
				return core::bc_make_nullable(bc_render_pass_config{ *l_pass->m_name, *l_pass->m_enabled });
			}
		}

		return {};
	}

	core::bc_vector<bc_render_pass_config> bc_global_config::get_render_pass_configs() const noexcept
	{
		core::bc_vector<bc_render_pass_config> l_passes;
		auto& l_json = *m_json;

		for (auto& l_pass : l_json->m_render_passes)
		{
			l_passes.push_back(bc_render_pass_config{ *l_pass->m_name, *l_pass->m_enabled });
		}

		return l_passes;
	}

	void bc_global_config::set_render_pass_config(const bc_render_pass_config& p_pass) noexcept
	{
		auto l_config = get_render_pass_config(p_pass.m_name);
		if (l_config.has_value())
		{
			l_config->m_enabled = p_pass.m_enabled;
		}
		else
		{
			auto& l_json = *m_json;
			auto& l_json_object = l_json->m_render_passes.new_entry();

			*l_json_object->m_name = p_pass.m_name;
			*l_json_object->m_enabled = p_pass.m_enabled;
		}
	}

	void bc_global_config::set_render_pass_configs(const core::bc_const_span<bc_render_pass_config>& p_passes) noexcept
	{
		auto& l_json = *m_json;
		l_json->m_render_passes.clear();

		for (const auto& l_pass : p_passes)
		{
			auto& l_pass_entry = l_json->m_render_passes.new_entry();
			*l_pass_entry->m_name = l_pass.m_name;
			*l_pass_entry->m_enabled = l_pass.m_enabled;
		}
	}

	core::bc_json_key_value* bc_global_config::load_json(const bcCHAR* p_json)
	{
		m_json = core::bc_make_unique<core::bc_json_document<bc_config_json>>();
		m_json->load(p_json);

		return &(*m_json)->m_key_values.get();
	}

	core::bc_string_frame bc_global_config::write_json()
	{
		return m_json->write_pretty_frame();
	}
}
