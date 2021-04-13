// [02/22/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/System/Input/bcConfigFile.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(bc_config_layout)
		{
			BC_JSON_VALUE_OP(bcFLOAT, lod_global_scale);
			BC_JSON_VALUE_OP(bcUINT32, lod_culling_index);
			BC_JSON_VALUE_OP(bcFLOAT, bullet_reference_mass);
			BC_JSON_ARRAY_OP(core::bc_string, counter_values);
			BC_JSON_VALUE_OP(core::bc_json_key_value, key_values);
		};
		
		class bc_global_config final : public bc_config_file
		{
		public:
			explicit bc_global_config(const bcECHAR* p_content_path);

			bc_global_config(bc_global_config&&) noexcept;

			~bc_global_config() override;

			bc_global_config& operator=(bc_global_config&&) noexcept;

			bcFLOAT get_lod_global_scale() const noexcept;

			bcUINT32 get_lod_culling_index() const noexcept;

			bcFLOAT get_bullet_reference_mass() const noexcept;
			
			const core::bc_json_array<core::bc_string>& get_counter_values() const noexcept;
			
			core::bc_json_array<core::bc_string>& get_counter_values() noexcept;
		
		private:
			core::bc_json_key_value* load_json(const bcCHAR* p_config_content) override;
			
			core::bc_string_frame write_json() override;

			core::bc_unique_ptr< core::bc_json_document< bc_config_layout >> m_json;
		};

		inline bc_global_config::bc_global_config(bc_global_config&&) noexcept = default;

		inline bc_global_config::~bc_global_config() = default;

		inline bc_global_config& bc_global_config::operator=(bc_global_config&&) noexcept = default;

		inline bcFLOAT bc_global_config::get_lod_global_scale() const noexcept
		{
			return *(*m_json)->m_lod_global_scale;
		}

		inline bcUINT32 bc_global_config::get_lod_culling_index() const noexcept
		{
			return *(*m_json)->m_lod_culling_index;
		}

		inline bcFLOAT bc_global_config::get_bullet_reference_mass() const noexcept
		{
			return *(*m_json)->m_bullet_reference_mass;
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

	extern BC_GAME_DLL game::bc_global_config* g_global_config;

	inline void set_global_config(game::bc_global_config& p_config)
	{
		g_global_config = &p_config;
	}

	inline game::bc_global_config& get_global_config()
	{
		return *g_global_config;
	}
}