// [02/02/2016 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatform/bcException.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatformImp/File/bcPath.h"
#include "CorePlatformImp/bcUtility.h"
#include <Shlwapi.h>
#include <Pathcch.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

namespace black_cat
{
	namespace core_platform
	{
		using path_string = bc_platform_path_pack<g_api_win32>::path_string;

		void _copy_current_path_to_buffer(const path_string& p_path, bcECHAR* p_buffer, bcSIZE p_buffer_size)
		{
			if (p_buffer_size < p_path.size() + 1)
			{
				throw bc_invalid_operation_exception("Provided buffer hasn't enough space");
			}

#ifdef BC_UNICODE
			wcscpy_s(p_buffer, p_buffer_size, p_path.c_str());
#else
			strcpy_s(p_buffer, p_buffer_size, p_path.c_str());
#endif
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>::bc_platform_path(const bcECHAR* p_path)
			: m_pack(path_string(p_path))
		{
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>::bc_platform_path(const bc_platform_path& p_other)
			: m_pack(p_other.m_pack.m_path)
		{
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>::bc_platform_path(bc_platform_path&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack.m_path))
		{
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>& bc_platform_path<g_api_win32>::operator=(const bc_platform_path& p_other)
		{
			m_pack.m_path = p_other.m_pack.m_path;
			return *this;
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>& bc_platform_path<g_api_win32>::operator=(bc_platform_path&& p_other) noexcept
		{
			m_pack.m_path = std::move(p_other.m_pack.m_path);
			return *this;
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_platform_path<g_api_win32>::~bc_platform_path()
		{	
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bcSIZE bc_platform_path<g_api_win32>::get_length() const
		{
			return m_pack.m_path.size();
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_path(bcECHAR* p_buffer, bcSIZE p_buffer_size) const
		{
			_copy_current_path_to_buffer(m_pack.m_path, p_buffer, p_buffer_size);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_directory(bcECHAR* p_buffer, bcSIZE p_buffer_size) const
		{
			_copy_current_path_to_buffer(m_pack.m_path, p_buffer, p_buffer_size);

			win_call(PathCchRemoveFileSpec(p_buffer, p_buffer_size));
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::set_directory(const bcECHAR* p_directory)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };

			get_filename(l_buffer, MAX_PATH);

			m_pack.m_path = p_directory;

			combine(bc_platform_path(l_buffer));
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_filename(bcECHAR* p_buffer, bcSIZE p_buffer_size) const
		{
			_copy_current_path_to_buffer(m_pack.m_path, p_buffer, p_buffer_size);

			PathStripPath(p_buffer);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_filename_without_extension(bcECHAR* p_buffer, bcSIZE p_buffer_size) const
		{
			_copy_current_path_to_buffer(m_pack.m_path, p_buffer, p_buffer_size);

			PathStripPath(p_buffer);
			PathRemoveExtension(p_buffer);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::set_filename(const bcECHAR* p_filename)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer, MAX_PATH);

			win_call(PathCchRemoveFileSpec(l_buffer, MAX_PATH));

			m_pack.m_path.assign(l_buffer);
			m_pack.m_path.append(bcL("\\"));
			m_pack.m_path.append(p_filename);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_file_extension(bcECHAR* p_buffer, bcSIZE p_buffer_size) const
		{
			auto* l_str_buffer = m_pack.m_path.c_str();
			PTSTR l_extension_pos = PathFindExtension(l_str_buffer);

			if (l_extension_pos != l_str_buffer)
			{
#ifdef BC_UNICODE
				wcscpy_s(p_buffer, p_buffer_size, l_extension_pos);
#else
				strcpy_s(p_buffer, p_buffer_size, l_extension_pos);
#endif
			}
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::set_file_extension(const bcECHAR* p_file_extension)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer, MAX_PATH);

			win_call(PathCchRenameExtension(l_buffer, MAX_PATH, p_file_extension));

			m_pack.m_path.assign(l_buffer);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::combine(const bc_platform_path& p_other)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer, MAX_PATH);

			win_call(PathCchCombine(l_buffer, MAX_PATH, m_pack.m_path.c_str(), p_other.m_pack.m_path.c_str()));

			m_pack.m_path.assign(l_buffer);
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_path<g_api_win32>::is_relative() const
		{
			return PathIsRelative(m_pack.m_path.c_str());
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_path<g_api_win32>::is_file() const
		{
			if (!exist())
				return false;

			DWORD l_attributes = GetFileAttributes(m_pack.m_path.c_str());

			if (l_attributes == INVALID_FILE_ATTRIBUTES)
			{
				win_call(false);
			}

			return (l_attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bool bc_platform_path<g_api_win32>::exist() const
		{
			return PathFileExists(m_pack.m_path.c_str());
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::delete_path() const noexcept(false)
		{
			if(is_file())
			{
				if(!DeleteFile(m_pack.m_path.c_str()))
				{
					win_call(false);
				}
			}
			else
			{
				if(!RemoveDirectory(m_pack.m_path.c_str()))
				{
					win_call(false);
				}
			}
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_program_path(bcECHAR* p_buffer, bcSIZE p_buffer_size)
		{
			if (GetModuleFileName(nullptr, p_buffer, p_buffer_size) == 0)
			{
				win_call(false);
			}
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		void bc_platform_path<g_api_win32>::get_absolute_path(const bcECHAR* p_relative_path, bcECHAR* p_buffer, bcSIZE p_buffer_size)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };

			get_program_path(l_buffer, MAX_PATH);

			bc_path l_path = bc_path(l_buffer);
			l_path.set_filename(p_relative_path);

#ifdef BC_UNICODE
			wcscpy_s(p_buffer, p_buffer_size, l_path.m_pack.m_path.c_str());
#else
			strcpy_s(p_buffer, p_buffer_size, l_path.m_pack.m_path.c_str());
#endif
		}

		template< >
		BC_COREPLATFORMIMP_DLL
		bc_path_info bc_platform_path<g_api_win32>::get_path_info()
		{
			bc_path_info l_info;
			l_info.m_max_path_length = MAX_PATH;
			l_info.m_net_drive = bcL("\\\\");
			l_info.m_path_sep = bcL("\\");
			l_info.m_rlv_dir = bcL(".\\");
			l_info.m_ext_sep = bcL(".");

			return l_info;
		}
	}
}