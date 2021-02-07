// [02/02/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "CorePlatformImp/bcUtility.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/File/bcPath.h"

#include <Shlwapi.h>
#include <Pathcch.h>
#include <Core/Container/bcString.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

namespace black_cat
{
	namespace platform
	{
		const bcECHAR* bc_platform_path<core_platform::g_api_win32>::s_net_drive = L"\\\\";
		const bcECHAR* bc_platform_path<core_platform::g_api_win32>::s_path_sep = L"\\";
		const bcECHAR* bc_platform_path<core_platform::g_api_win32>::s_rlv_dir = L".\\";
		const bcECHAR* bc_platform_path<core_platform::g_api_win32>::s_ext_sep = L".";

		void _copy_current_path_to_buffer(core::bc_estring p_path, bcECHAR* p_buffer)
		{
			BC_ASSERT(p_path.size() < MAX_PATH);

#ifdef BC_UNICODE
			std::wcscpy(p_buffer, p_path.c_str());
#else
			std::strcpy(p_buffer, p_str.c_str());
#endif
		}

		template< >      
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>::bc_platform_path(const bcECHAR* p_path)
			: m_pack(core::bc_estring(p_path))
		{
		};

		template< >
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>::bc_platform_path(core::bc_estring p_path)
			: m_pack(std::move(p_path))
		{
		};

		template< >
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>::bc_platform_path(const bc_platform_path& p_other)
			: m_pack(p_other.m_pack)
		{
		};

		template< >
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>::bc_platform_path(bc_platform_path&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		};

		template< >
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>& bc_platform_path<core_platform::g_api_win32>::operator=(const bc_platform_path& p_other)
		{
			m_pack = p_other.m_pack;

			return *this;
		};

		template< >
		BC_PLATFORMIMP_DLL bc_platform_path<core_platform::g_api_win32>& bc_platform_path<core_platform::g_api_win32>::operator=(bc_platform_path&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_path() const
		{
			return core::bc_estring(m_pack.m_path.c_str());
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_directory() const
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);
			
			PathCchRemoveFileSpec(l_buffer, MAX_PATH);

			return core::bc_estring(l_buffer);
		};

		template< >
		BC_PLATFORMIMP_DLL void bc_platform_path<core_platform::g_api_win32>::set_directory(core::bc_estring p_directory)
		{
			core::bc_estring l_filename = get_filename();
			
			m_pack.m_path = std::move(p_directory);

			combine(bc_platform_path(l_filename.c_str()));
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_filename() const
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);

			PathStripPath(l_buffer);

			return core::bc_estring(l_buffer);
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_filename_without_extension() const
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);

			PathStripPath(l_buffer);
			PathRemoveExtension(l_buffer);

			return core::bc_estring(l_buffer);
		};

		template< >
		BC_PLATFORMIMP_DLL void bc_platform_path<core_platform::g_api_win32>::set_filename(core::bc_estring p_filename)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);

			PathCchRemoveFileSpec(l_buffer, MAX_PATH);

			m_pack.m_path.assign(l_buffer);
			m_pack.m_path.append(s_path_sep);
			m_pack.m_path.append(p_filename);
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_file_extension() const
		{
			core::bc_estring l_result;
			auto* l_str_buffer = m_pack.m_path.c_str();
			PTSTR l_extension_pos = PathFindExtension(l_str_buffer);
			
			if (l_extension_pos != l_str_buffer)
				l_result.assign(l_extension_pos);
			
			return l_result;
		};

		template< >
		BC_PLATFORMIMP_DLL void bc_platform_path<core_platform::g_api_win32>::set_file_extension(core::bc_estring p_file_extension)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);

			PathCchRenameExtension(l_buffer, MAX_PATH, p_file_extension.c_str());

			m_pack.m_path.assign(l_buffer);
		};

		template< >
		BC_PLATFORMIMP_DLL void bc_platform_path<core_platform::g_api_win32>::combine(bc_platform_path& p_other)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };
			_copy_current_path_to_buffer(m_pack.m_path, l_buffer);

			PathCchCombine(l_buffer, MAX_PATH, m_pack.m_path.c_str(), p_other.m_pack.m_path.c_str());

			m_pack.m_path.assign(l_buffer);
		};

		template< >
		BC_PLATFORMIMP_DLL bool bc_platform_path<core_platform::g_api_win32>::is_relative() const
		{
			return PathIsRelative(m_pack.m_path.c_str());
		};

		template< >
		BC_PLATFORMIMP_DLL bool bc_platform_path<core_platform::g_api_win32>::is_file() const
		{
			if (!exist())
				return false;

			DWORD l_attributes = GetFileAttributes(m_pack.m_path.c_str());
			
			if (l_attributes == INVALID_FILE_ATTRIBUTES)
				core_platform::win_call(false);

			if (l_attributes & FILE_ATTRIBUTE_DIRECTORY)
				return false;

			return true;
		};

		template< >
		BC_PLATFORMIMP_DLL bool bc_platform_path<core_platform::g_api_win32>::exist() const
		{
			return PathFileExists(m_pack.m_path.c_str());
		};

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_program_path()
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };

			if(GetModuleFileName(nullptr, l_buffer, MAX_PATH) == 0)
			{
				core_platform::win_call(false);
			}

			return core::bc_estring(l_buffer);
		}

		template< >
		BC_PLATFORMIMP_DLL core::bc_estring bc_platform_path<core_platform::g_api_win32>::get_absolute_path(const bcECHAR* p_relative_path)
		{
			bcECHAR l_buffer[MAX_PATH] = { '\0' };

			if (GetModuleFileName(nullptr, l_buffer, MAX_PATH) == 0)
			{
				core_platform::win_call(false);
			}

			bc_path l_path = bc_path(l_buffer);
			l_path.set_filename(p_relative_path);

			return l_path.get_path();
		}
	}
}