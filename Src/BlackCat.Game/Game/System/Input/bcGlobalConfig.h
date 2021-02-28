// [02/22/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/System/Input/bcConfigFile.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(bc_config_layout)
		{
			BC_JSON_VALUE_OP(bcFLOAT, lod_global_scale);
			BC_JSON_VALUE_OP(bcUINT32, lod_culling_index);
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
	}

	extern game::bc_global_config* g_global_config;

	inline void set_global_config(game::bc_global_config& p_config)
	{
		g_global_config = &p_config;
	}

	inline game::bc_global_config& get_global_config()
	{
		return *g_global_config;
	}
}