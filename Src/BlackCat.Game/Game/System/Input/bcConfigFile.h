// [20/12/2020 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcPath.h"
#include "Core/File/bcFileStream.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_json_key_value;
		class bc_parameter_pack;
		class bc_any;
	}

	namespace game
	{
		class BC_GAME_DLL bc_config_file
		{
		public:
			virtual ~bc_config_file() = default;

			core::bc_estring_view get_path() const noexcept;

			bc_config_file& add_if_not_exist_config_key(core::bc_string_view p_key, core::bc_any p_value) noexcept;

			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, core::bc_any p_value) noexcept;
			
			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector2f& p_value) noexcept;

			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector2i& p_value) noexcept;
			
			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector3f& p_value) noexcept;

			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector3i& p_value) noexcept;
			
			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector4f& p_value) noexcept;

			bc_config_file& add_or_update_config_key(core::bc_string_view p_key, const core::bc_vector4i& p_value) noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_any& p_value) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_any& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector2f& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector2f& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector2i& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector2i& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector3f& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector3f& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector3i& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector3i& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector4f& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector4f& p_value, bool& p_succeeded) const noexcept;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector4i& p_value) const;

			const bc_config_file& read_config_key(core::bc_string_view p_key, core::bc_vector4i& p_value, bool& p_succeeded) const noexcept;

			void reload();

			void flush_changes();

		protected:
			bc_config_file();

			bc_config_file(bc_config_file&& p_other) noexcept = default;

			bc_config_file& operator=(bc_config_file&& p_other) noexcept = default;

			void load(const core::bc_path& p_path);
			
		private:
			virtual core::bc_json_key_value* load_json(const bcCHAR* p_json) = 0;

			virtual core::bc_string_frame write_json() = 0;

			core::bc_estring m_path;
			core::bc_file_stream m_stream;
			core::bc_json_key_value* m_key_values;
		};
	}
}