// [08/07/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/File/bcPath.h"

namespace black_cat
{
	namespace core
	{
		bc_path::bc_path(const bcECHAR* p_path)
			: m_path(p_path)
		{
		}

		bc_path::bc_path(bc_estring_view p_path)
			: m_path(p_path.data())
		{
		}

		bc_path::bc_path(const bc_path& p_other) = default;

		bc_path::bc_path(bc_path&& p_other) noexcept = default;

		bc_path& bc_path::operator=(const bc_path& p_other) = default;

		bc_path& bc_path::operator=(bc_path&& p_other) noexcept = default;

		bc_path::~bc_path() = default;

		bc_estring bc_path::get_directory() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_directory(&l_buffer[0], l_buffer.size());

			return bc_estring(l_buffer.c_str());
		}

		bc_estring_frame bc_path::get_directory_frame() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_directory(&l_buffer[0], l_buffer.size());

			return bc_estring_frame(l_buffer.c_str());
		}

		bc_path& bc_path::set_directory(const bcECHAR* p_directory)
		{
			m_path.set_directory(p_directory);

			return *this;
		}

		bc_estring bc_path::get_filename() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_filename(&l_buffer[0], l_buffer.size());

			return bc_estring(l_buffer.c_str());
		}

		bc_estring_frame bc_path::get_filename_frame() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_filename(&l_buffer[0], l_buffer.size());

			return bc_estring_frame(l_buffer.c_str());
		}

		bc_estring bc_path::get_filename_without_extension() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_filename_without_extension(&l_buffer[0], l_buffer.size());

			return bc_estring(l_buffer.c_str());
		}

		bc_estring_frame bc_path::get_filename_without_extension_frame() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_filename_without_extension(&l_buffer[0], l_buffer.size());

			return bc_estring_frame(l_buffer.c_str());
		}

		bc_path& bc_path::set_filename(const bcECHAR* p_filename)
		{
			m_path.set_filename(p_filename);

			return *this;
		}

		bc_estring bc_path::get_file_extension() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_file_extension(&l_buffer[0], l_buffer.size());

			return bc_estring(l_buffer.c_str());
		}

		bc_estring_frame bc_path::get_file_extension_frame() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_file_extension(&l_buffer[0], l_buffer.size());

			return bc_estring_frame(l_buffer.c_str());
		}

		bc_path& bc_path::set_file_extension(const bcECHAR* p_file_extension)
		{
			m_path.set_file_extension(p_file_extension);

			return *this;
		}

		bc_path& bc_path::combine(const bc_path& p_other)
		{
			m_path.combine(p_other.m_path);

			return *this;
		}

		bc_estring bc_path::get_string() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_string(&l_buffer[0], l_buffer.size());

			return bc_estring(l_buffer.c_str());
		}

		bc_estring_frame bc_path::get_string_frame() const
		{
			bc_estring_frame l_buffer(m_path.get_length() + 1, ' ');

			m_path.get_string(&l_buffer[0], l_buffer.size());

			return bc_estring_frame(l_buffer.c_str());
		}
		
		bool bc_path::is_relative() const
		{
			return m_path.is_relative();
		}

		bool bc_path::is_file() const
		{
			return m_path.is_file();
		}

		bool bc_path::exist() const
		{
			return m_path.exist();
		}

		void bc_path::create_directory() const noexcept(false)
		{
			m_path.create_directory();
		}

		void bc_path::delete_path() const noexcept(false)
		{
			m_path.delete_path();
		}

		bc_path bc_path::get_program_path()
		{
			const auto l_max_path = get_path_info().m_max_path_length;
			bc_estring_frame l_buffer(l_max_path, ' ');
			
			platform::bc_path::get_program_path(&l_buffer[0], l_max_path);

			return { l_buffer.c_str() };
		}

		bc_path bc_path::get_absolute_path(const bcECHAR* p_relative_path)
		{
			const auto l_max_path = get_path_info().m_max_path_length;
			bc_estring_frame l_buffer(l_max_path, ' ');

			platform::bc_path::get_absolute_path(p_relative_path, &l_buffer[0], l_max_path);

			return { l_buffer.c_str() };
		}

		bc_path_info bc_path::get_path_info()
		{
			return platform::bc_path::get_path_info();
		}
	}
}