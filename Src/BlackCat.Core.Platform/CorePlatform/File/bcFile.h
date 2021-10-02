// [03/26/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"

namespace black_cat
{
	namespace core_platform
	{
		enum class bc_file_status
		{
			open,				// File opened and ready
			close,				// File is closed
			error				// File isn't open and error occurred during file opening
		};

		enum class bc_file_mode
		{
			create,				// Create file, if it doesn't exist
			create_overwrite,	// Create file, if exist overwrite it
			create_append,		// Create file, if exist seek to end of it
			open,				// Open a file, if exist
			open_create,		// Open a file, if doesn't exist, create one
			open_truncate		// Open a file, if exist and truncate it
		};

		enum class bc_file_access
		{
			read,
			write,
			read_write
		};

		enum class bc_file_sharing
		{
			none,
			read,
			write,
			read_write
		};

		enum class bc_file_seek
		{
			start,
			current,
			end
		};

		template< bc_platform >
		struct bc_platform_file_pack
		{
		};

		template< bc_platform TPlatform >
		class bc_platform_file : bc_no_copy
		{
		public:
			using platform_pack = bc_platform_file_pack< TPlatform >;

		public:
			bc_platform_file() noexcept;

			bc_platform_file(bc_platform_file&&) noexcept;

			~bc_platform_file();

			bc_platform_file& operator=(bc_platform_file&&) noexcept;

			bc_file_status get_status() const noexcept;

			bool is_open() const noexcept;

			bool is_close() const noexcept;

			/**
			 * \brief This function does not throw exception but if it fail, any subsequence calls to object will throw exception
			 * \param p_file 
			 * \param p_mode 
			 * \param p_access 
			 * \param p_sharing 
			 * \return 
			 */
			bool open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access, bc_file_sharing p_sharing) noexcept;

			bool open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access) noexcept;

			/**
			 * \brief Open an existing file for reading(open mode)
			 * \param p_file 
			 * \return 
			 */
			bool open_read(const bcECHAR* p_file) noexcept;

			/**
			 * \brief Open an existing file or create a new file for writing(create_overwrite mode)
			 * \param p_file 
			 * \return 
			 */
			bool open_write(const bcECHAR* p_file) noexcept;

			/**
			 * \brief Move file pointer by offset(bytes) and return new location from beginning of file.
			 * \n Seeking to a location beyond the length of file is valid.
			 * \param p_seek_location 
			 * \param p_offset 
			 * \return 
			 */
			bcUINT64 set_pointer(bc_file_seek p_seek_location, bcINT64 p_offset);

			/**
			 * \brief Get current file pointer
			 * \return 
			 */
			bcUINT64 get_pointer() const;

			/**
			 * \brief Get end of file pointer
			 * \return 
			 */
			bcUINT64 get_eof_pointer() const;

			/**
			 * \brief Return number of bytes that has been read
			 * \param p_buffer 
			 * \param p_bytes_to_read 
			 * \return 
			 */
			bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const;

			/**
			 * \brief Return number of bytes that has been written
			 * \param p_buffer 
			 * \param p_bytes_to_write 
			 * \return 
			 */
			bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write);

			/**
			 * \brief Return size of file in bytes
			 * \return 
			 */
			bcUINT64 size() const;

			void flush();

			void close();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		template< bc_platform TPlatform >
		bool bc_platform_file< TPlatform >::is_open() const noexcept
		{
			return get_status() == bc_file_status::open;
		}

		template< bc_platform TPlatform >
		bool bc_platform_file< TPlatform >::is_close() const noexcept
		{
			return get_status() == bc_file_status::close;
		}

		using bc_file = bc_platform_file< g_current_platform >;
	}
}