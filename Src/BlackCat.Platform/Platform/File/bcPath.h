// [02/02/2016 MRB]

#pragma once

#include "Platform/PlatformPCH.h"

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform >
		struct bc_platform_path_pack
		{
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_path
		{
		public:
			static const bcECHAR* s_net_drive;
			static const bcECHAR* s_path_sep;
			static const bcECHAR* s_rlv_dir;
			static const bcECHAR* s_ext_sep;
			using platform_pack = bc_platform_path_pack<TPlatform>;

		public:
			explicit bc_platform_path(const bcECHAR* p_path);

			explicit bc_platform_path(core::bc_estring p_path);

			bc_platform_path(const bc_platform_path& p_other);

			bc_platform_path(bc_platform_path&& p_other);

			bc_platform_path& operator=(const bc_platform_path& p_other);

			bc_platform_path& operator=(bc_platform_path&& p_other);

			core::bc_estring get_path() const;

			core::bc_estring get_directory() const;

			void set_directory(core::bc_estring p_directory);

			core::bc_estring get_filename() const;

			core::bc_estring get_filename_without_extension() const;

			// Change file name with it's extension
			void set_filename(core::bc_estring p_filename);

			core::bc_estring get_file_extension() const;

			void set_file_extension(core::bc_estring p_file_extension);

			// If second path is fully qualfied, it will be replaced with current path 
			void combine(bc_platform_path& p_other);

			bool is_relative() const;

			// If path doesn't exist, it will return false
			bool is_file() const;

			bool exist() const;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			static core::bc_estring get_program_path();

			static core::bc_estring get_absolute_path(const bcECHAR* p_relative_path);

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_path = bc_platform_path<core_platform::g_current_platform>;
	}
}