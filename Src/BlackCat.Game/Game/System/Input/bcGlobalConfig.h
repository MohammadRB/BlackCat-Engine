// [22/02/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Memory/bcPtr.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/System/Input/bcConfigFile.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	BC_JSON_STRUCTURE(bc_config_layout)
	{
		BC_JSON_VALUE_OP(core::bc_string, preferred_gpu);
		BC_JSON_ARRAY_OP(core::bc_string, log_types);
		BC_JSON_VALUE_OP(bcFLOAT, scene_global_scale);
		BC_JSON_VALUE_OP(bcFLOAT, scene_global_view_distance_scale);
		BC_JSON_VALUE_OP(bool, scene_graph_debug_draw);
		BC_JSON_VALUE_OP(bcUINT32, scene_graph_actors_pool_capacity);
		BC_JSON_VALUE_OP(bcFLOAT, scene_bullet_reference_mass);
		BC_JSON_VALUE_OP(core::bc_string, network_client_name);
		BC_JSON_ARRAY_OP(core::bc_string, counter_values);
		BC_JSON_VALUE_OP(core::bc_json_key_value, key_values);
	};
	
	class bc_global_config final : public bc_config_file
	{
	public:
		explicit bc_global_config(const bcECHAR* p_content_path, const bcECHAR* p_config_file_name = bcL("config"));

		bc_global_config(bc_global_config&&) noexcept;

		~bc_global_config() override;

		bc_global_config& operator=(bc_global_config&&) noexcept;

		core::bc_string_view get_preferred_gpu() const noexcept;

		core::bc_vector<core::bc_string> get_log_types() const noexcept;

		bcFLOAT get_scene_global_scale() const noexcept;
		
		bcFLOAT get_scene_global_view_distance_scale() const noexcept;
		
		bool get_scene_graph_debug_draw() const noexcept;

		bcUINT32 get_scene_graph_actors_pool_capacity() const noexcept;

		bcFLOAT get_scene_bullet_reference_mass() const noexcept;

		core::bc_string_view get_network_client_name() const noexcept;

		void set_network_client_name(core::bc_string p_name) const noexcept;
		
		const core::bc_json_array<core::bc_string>& get_counter_values() const noexcept;
		
		core::bc_json_array<core::bc_string>& get_counter_values() noexcept;
	
	private:
		core::bc_json_key_value* load_json(const bcCHAR* p_json) override;
		
		core::bc_string_frame write_json() override;

		core::bc_unique_ptr<core::bc_json_document<bc_config_layout>> m_json;
	};

	inline bc_global_config::bc_global_config(bc_global_config&&) noexcept = default;

	inline bc_global_config::~bc_global_config() = default;

	inline bc_global_config& bc_global_config::operator=(bc_global_config&&) noexcept = default;

	inline core::bc_string_view bc_global_config::get_preferred_gpu() const noexcept
	{
		return *(*m_json)->m_preferred_gpu;
	}

	inline bcFLOAT bc_global_config::get_scene_global_scale() const noexcept
	{
		return *(*m_json)->m_scene_global_scale;
	}
	
	inline bcFLOAT bc_global_config::get_scene_global_view_distance_scale() const noexcept
	{
		return *(*m_json)->m_scene_global_view_distance_scale;
	}
	
	inline bool bc_global_config::get_scene_graph_debug_draw() const noexcept
	{
		return *(*m_json)->m_scene_graph_debug_draw;
	}

	inline bcUINT32 bc_global_config::get_scene_graph_actors_pool_capacity() const noexcept
	{
		return *(*m_json)->m_scene_graph_actors_pool_capacity;
	}

	inline bcFLOAT bc_global_config::get_scene_bullet_reference_mass() const noexcept
	{
		return *(*m_json)->m_scene_bullet_reference_mass;
	}

	inline core::bc_string_view bc_global_config::get_network_client_name() const noexcept
	{
		return *(*m_json)->m_network_client_name;
	}

	inline void bc_global_config::set_network_client_name(core::bc_string p_name) const noexcept
	{
		(*m_json)->m_network_client_name.set(std::move(p_name));
	}

	inline const core::bc_json_array<core::bc_string>& bc_global_config::get_counter_values() const noexcept
	{
		return (*m_json)->m_counter_values;
	}

	inline core::bc_json_array<core::bc_string>& bc_global_config::get_counter_values() noexcept
	{
		return (*m_json)->m_counter_values;
	}
}

namespace black_cat
{
	extern BC_GAME_DLL game::bc_global_config* g_global_config;

	inline game::bc_global_config& bc_get_global_config()
	{
		return *g_global_config;
	}
}