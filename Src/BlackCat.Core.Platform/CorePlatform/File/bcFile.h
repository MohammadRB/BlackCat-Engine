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
			error				// File isn't open and error occured during file opening
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

			// This function doesn't throw exception but if it fail, any subsequence calls to object will
			// throw exception
			bool open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access, bc_file_sharing p_sharing) noexcept;

			bool open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access) noexcept;

			// Open an existing file for reading(open mode)
			bool open_read(const bcECHAR* p_file) noexcept;

			// Open an existing file or create a new file for writing(open_create mode)
			bool open_write(const bcECHAR* p_file) noexcept;

			// Move file pointer by offset(bytes) and return new location from beginning of file
			bcUINT64 set_pointer(bc_file_seek p_seek_location, bcINT64 p_offset);

			// Get current file pointer
			bcUINT64 get_pointer() const;

			// Get end of file pointer
			bcUINT64 get_eof_pointer() const;

			// Return number of bytes that has been read
			bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read);

			// Return number of bytes that has been written
			bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write);

			// Return size of file in bytes
			bcUINT64 size() const;

			void flush();

			void close();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

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