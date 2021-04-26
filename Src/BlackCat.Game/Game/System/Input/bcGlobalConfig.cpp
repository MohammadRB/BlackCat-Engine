// [02/22/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcGlobalConfig.h"

namespace black_cat
{
	game::bc_global_config* g_global_config = nullptr;
	
	namespace game
	{
		bc_global_config::bc_global_config(const bcECHAR* p_content_path)
			: bc_config_file()
		{
			m_json = core::bc_make_unique<core::bc_json_document<bc_config_layout>>();
			load(p_content_path, bcL("config.json"));

			auto& l_json = *m_json;
			if(!l_json->m_lod_global_scale.get_had_value())
			{
				*l_json->m_lod_global_scale = 4.0f;
			}
			if (!l_json->m_lod_culling_index.get_had_value())
			{
				*l_json->m_lod_culling_index = 6;
			}
			if(!l_json->m_scene_graph_actors_pool_capacity.get_had_value())
			{
				*l_json->m_scene_graph_actors_pool_capacity = 2000;
			}
			if(!l_json->m_bullet_reference_mass.get_had_value())
			{
				*l_json->m_bullet_reference_mass = 0.1f;
			}
			if(!l_json->m_counter_values.get_had_value())
			{
				l_json->m_counter_values.new_entry().set("fps");
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

		core::bc_json_key_value* bc_global_config::load_json(const bcCHAR* p_config_content)
		{
			m_json->load(p_config_content);
			return &(*m_json)->m_key_values.get();
		}

		core::bc_string_frame bc_global_config::write_json()
		{
			return m_json->write();
		}
	}
}