// [02/02/2016 MRB]

#pragma once

#include "CorePlatform/bcExport.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat
{
	namespace core_platform
	{
		struct BC_COREPLATFORM_DLL bc_path_info
		{
			bcSIZE m_max_path_length;
			bcECHAR* m_net_drive;
			bcECHAR* m_path_sep;
			bcECHAR* m_rlv_dir;
			bcECHAR* m_ext_sep;
		};

		template< bc_platform >
		struct bc_platform_path_pack
		{
		};

		template< bc_platform TPlatform >
		class bc_platform_path
		{
		public:
			using platform_pack = bc_platform_path_pack<TPlatform>;

		public:
			explicit bc_platform_path(const bcECHAR* p_path);

			bc_platform_path(const bc_platform_path& p_other);

			bc_platform_path(bc_platform_path&& p_other) noexcept;

			bc_platform_path& operator=(const bc_platform_path& p_other);

			bc_platform_path& operator=(bc_platform_path&& p_other) noexcept;

			~bc_platform_path();

			bcSIZE get_length() const;

			void get_directory(bcECHAR* p_buffer, bcSIZE p_buffer_size) const;

			void set_directory(const bcECHAR* p_directory);

			void get_filename(bcECHAR* p_buffer, bcSIZE p_buffer_size) const;

			void get_filename_without_extension(bcECHAR* p_buffer, bcSIZE p_buffer_size) const;

			/**
			 * \brief Change file name with it's extension
			 * \param p_filename 
			 */
			void set_filename(const bcECHAR* p_filename);

			void get_file_extension(bcECHAR* p_buffer, bcSIZE p_buffer_size) const;

			void set_file_extension(const bcECHAR* p_file_extension);

			/**
			 * \brief If second path is fully qualified, it will be replaced with current path 
			 * \param p_other 
			 */
			void combine(const bc_platform_path& p_other);

			void get_string(bcECHAR* p_buffer, bcSIZE p_buffer_size) const;
			
			bool is_relative() const;

			/**
			 * \brief If path doesn't exist, it will return false
			 * \return 
			 */
			bool is_file() const;

			bool exist() const;

			/**
			 * \brief Delete directory or file
			 */
			void delete_path() const noexcept(false);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

			static void get_program_path(bcECHAR* p_buffer, bcSIZE p_buffer_size);

			static void get_absolute_path(const bcECHAR* p_relative_path, bcECHAR* p_buffer, bcSIZE p_buffer_size);

			static bc_path_info get_path_info();

		private:
			platform_pack m_pack;
		};

		using bc_path = bc_platform_path<g_current_platform>;
	}
}