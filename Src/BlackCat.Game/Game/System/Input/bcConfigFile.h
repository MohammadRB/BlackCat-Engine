// [12/20/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/File/bcFileStream.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/File/bcJsonDocument.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_parameter_pack;
		class bc_any;
	}

	namespace game
	{
		BC_JSON_STRUCTURE(bc_config_layout)
		{
			BC_JSON_VALUE(core::bc_json_key_value, key_values);
		};

		class BC_GAME_DLL bc_config_file
		{
		public:
			bc_config_file(const bcECHAR* p_content_path, const bcECHAR* p_config_name);

			bc_config_file(bc_config_file&& p_other) noexcept = default;

			~bc_config_file() = default;

			bc_config_file& operator=(bc_config_file&& p_other) noexcept = default;

			bc_config_file& add_or_update_config_key(const bcCHAR* p_key, core::bc_any p_value) noexcept;

			bc_config_file& read_config_key(const bcCHAR* p_key, core::bc_any& p_value) noexcept;

			bc_config_file& read_config_key(const bcCHAR* p_key, core::bc_any& p_value, bool& p_succeeded) noexcept;

			void flush_changes();
			
		private:
			core::bc_file_stream m_stream;
			core::bc_unique_ptr< core::bc_json_document< bc_config_layout > > m_json;
		};
	}
}