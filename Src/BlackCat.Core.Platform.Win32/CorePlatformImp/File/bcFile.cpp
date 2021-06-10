// [03/26/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatform/bcException.h"
#include "CorePlatformImp/bcUtility.h"
#include "CorePlatformImp/File/bcFile.h"
#include <fileapi.h>

namespace black_cat
{
	namespace core_platform
	{
		void _check_file_is_open(HANDLE p_file, DWORD p_error_code)
		{
			if (p_file == INVALID_HANDLE_VALUE && p_error_code != 0)
			{
				win_call(p_error_code);
			}
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_file<g_api_win32>::bc_platform_file() noexcept
			: m_pack()
		{
			m_pack.m_file = INVALID_HANDLE_VALUE;
			m_pack.m_error_code = 0;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_file<g_api_win32>::bc_platform_file(bc_platform_file&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
			p_other.m_pack.m_file = INVALID_HANDLE_VALUE;
			p_other.m_pack.m_error_code = 0;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_file<g_api_win32>::~bc_platform_file()
		{
			if (get_status() == bc_file_status::open)
			{
				close();
			}
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_platform_file<g_api_win32>& bc_platform_file<g_api_win32>::operator=(bc_platform_file&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);

			p_other.m_pack.m_file = INVALID_HANDLE_VALUE;
			p_other.m_pack.m_error_code = 0;

			return *this;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bc_file_status bc_platform_file<g_api_win32>::get_status() const noexcept
		{
			if (m_pack.m_file != INVALID_HANDLE_VALUE && m_pack.m_error_code == 0)
			{
				return bc_file_status::open;
			}

			if (m_pack.m_file == INVALID_HANDLE_VALUE && m_pack.m_error_code != 0)
			{
				return bc_file_status::error;
			}

			if (m_pack.m_file == INVALID_HANDLE_VALUE && m_pack.m_error_code == 0)
			{
				return bc_file_status::close;
			}

			BC_ASSERT(false);
			return bc_file_status::error;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file<g_api_win32>::open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access, bc_file_sharing p_sharing) noexcept
		{
			HANDLE l_file;

			if (get_status() == bc_file_status::open)
			{
				close();
			}

			if ((l_file = CreateFile
				(
					p_file,
					bc_platform_cast(p_access),
					bc_platform_cast(p_sharing),
					nullptr,
					bc_platform_cast(p_mode),
					FILE_ATTRIBUTE_NORMAL,
					nullptr
				)) == INVALID_HANDLE_VALUE)
			{
				m_pack.m_error_code = GetLastError();
				return false;
			}

			m_pack.m_file = l_file;

			if (p_mode == bc_file_mode::create_append)
			{
				set_pointer(bc_file_seek::end, 0);
			}

			return true;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file<g_api_win32>::open(const bcECHAR* p_file, bc_file_mode p_mode, bc_file_access p_access) noexcept
		{
			return open(p_file, p_mode, p_access, p_access == bc_file_access::read ? bc_file_sharing::read : bc_file_sharing::none);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file<g_api_win32>::open_read(const bcECHAR* p_file) noexcept
		{
			return open(p_file, bc_file_mode::open, bc_file_access::read, bc_file_sharing::read);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_file<g_api_win32>::open_write(const bcECHAR* p_file) noexcept
		{
			return open(p_file, bc_file_mode::create_overwrite, bc_file_access::write, bc_file_sharing::none);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcUINT64 bc_platform_file<g_api_win32>::set_pointer(bc_file_seek p_seek_location, bcINT64 p_offset)
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			LARGE_INTEGER l_move_offset;
			LARGE_INTEGER l_new_pointer;

			l_move_offset.QuadPart = static_cast<LONGLONG>(p_offset);
			win_call(SetFilePointerEx(m_pack.m_file, l_move_offset, &l_new_pointer, bc_platform_cast(p_seek_location)) != false);

			return static_cast<bcUINT64>(l_new_pointer.QuadPart);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcUINT64 bc_platform_file<g_api_win32>::get_pointer() const
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			return const_cast<bc_platform_file*>(this)->set_pointer(bc_file_seek::current, 0);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcUINT64 bc_platform_file<g_api_win32>::get_eof_pointer() const
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			const bcUINT64 l_current = get_pointer();
			const bcUINT64 l_end = const_cast<bc_platform_file*>(this)->set_pointer(bc_file_seek::end, 0);

			const_cast<bc_platform_file*>(this)->set_pointer(bc_file_seek::start, l_current);

			return l_end;
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcSIZE bc_platform_file<g_api_win32>::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read)
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			DWORD l_number_read;
			win_call(ReadFile(m_pack.m_file, p_buffer, p_bytes_to_read, &l_number_read, nullptr) != false);

			return static_cast<bcSIZE>(l_number_read);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcSIZE bc_platform_file<g_api_win32>::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			DWORD l_number_wrote;
			win_call(WriteFile(m_pack.m_file, p_buffer, p_bytes_to_write, &l_number_wrote, nullptr) != false);

			return static_cast< bcSIZE >(l_number_wrote);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		bcUINT64 bc_platform_file<g_api_win32>::size() const
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			LARGE_INTEGER l_size;
			win_call(GetFileSizeEx(m_pack.m_file, &l_size) != false);

			return static_cast< bcUINT64 >(l_size.QuadPart);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_file<g_api_win32>::flush()
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			win_call(FlushFileBuffers(m_pack.m_file) != false);
		}

		template<>
		BC_COREPLATFORMIMP_DLL
		void bc_platform_file<g_api_win32>::close()
		{
			_check_file_is_open(m_pack.m_file, m_pack.m_error_code);

			win_call(CloseHandle(m_pack.m_file) != false);
			m_pack.m_file = INVALID_HANDLE_VALUE;
		}
	}
}