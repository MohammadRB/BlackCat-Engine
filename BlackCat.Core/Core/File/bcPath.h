// [07/08/2016 MRB]

#pragma once

#include "CorePlatformImp/File/bcPath.h"
#include "Core/bcExport.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		using core_platform::bc_path_info;

		// Same as core_platform::bc_platform_path but provide convenience api
		class BC_CORE_DLL bc_path
		{
		public:
			explicit bc_path(const bcECHAR* p_path);

			bc_path(const bc_path& p_other);

			bc_path(bc_path&& p_other);

			bc_path& operator=(const bc_path& p_other);

			bc_path& operator=(bc_path&& p_other);

			bc_estring get_path() const;

			bc_estring get_directory() const;

			bc_path& set_directory(bc_estring p_directory);

			bc_estring get_filename() const;

			bc_estring get_filename_without_extension() const;

			// Change file name with it's extension
			bc_path& set_filename(bc_estring p_filename);

			bc_estring get_file_extension() const;

			bc_path& set_file_extension(bc_estring p_file_extension);

			// If second path is fully qualfied, it will be replaced with current path 
			bc_path& combine(bc_path& p_other);

			bool is_relative() const;

			// If path doesn't exist, it will return false
			bool is_file() const;

			bool exist() const;

			void delete_path() const noexcept(false);

			static bc_estring get_program_path();

			static bc_estring get_absolute_path(const bcECHAR* p_relative_path);

			static bc_path_info get_path_info();

		protected:

		private:
			core_platform::bc_path m_path;
		};
	}
}
