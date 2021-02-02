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

		class BC_CORE_DLL bc_path
		{
		public:
			explicit bc_path(const bcECHAR* p_path);

			bc_path(const bc_path& p_other);

			bc_path(bc_path&& p_other) noexcept;

			bc_path& operator=(const bc_path& p_other);

			bc_path& operator=(bc_path&& p_other) noexcept;

			~bc_path();

			bc_estring get_directory() const;
			
			bc_estring_frame get_directory_frame() const;

			bc_path& set_directory(const bcECHAR* p_directory);

			bc_estring get_filename() const;
			
			bc_estring_frame get_filename_frame() const;

			bc_estring get_filename_without_extension() const;
			
			bc_estring_frame get_filename_without_extension_frame() const;

			/**
			 * \brief Change file name with it's extension
			 * \param p_filename 
			 * \return 
			 */
			bc_path& set_filename(const bcECHAR* p_filename);

			bc_estring get_file_extension() const;
			
			bc_estring_frame get_file_extension_frame() const;

			bc_path& set_file_extension(const bcECHAR* p_file_extension);

			/**
			 * \brief If second path is fully qualified, it will be replaced with current path 
			 * \param p_other 
			 * \return 
			 */
			bc_path& combine(const bc_path& p_other);

			bc_estring get_string() const;

			bc_estring_frame get_string_frame() const;
			
			bool is_relative() const;

			/**
			 * \brief If path does not exist, it will return false
			 * \return 
			 */
			bool is_file() const;

			bool exist() const;

			void delete_path() const noexcept(false);

			static bc_estring get_program_path();

			static bc_estring get_absolute_path(const bcECHAR* p_relative_path);

			static bc_path_info get_path_info();

		private:
			core_platform::bc_path m_path;
		};
	}
}