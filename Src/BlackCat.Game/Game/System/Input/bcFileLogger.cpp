// [10/01/2021 MRB]

#include "Game/GamePCH.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include "Core/File/bcPath.h"
#include "Game/System/Input/bcFileLogger.h"

namespace black_cat
{
	namespace game
	{
		bc_file_logger::bc_file_logger(const bcECHAR* p_path, const bcECHAR* p_app_name)
			: m_buffer_length(0)
		{
			auto l_time = std::time(nullptr);
			auto* l_local_time = std::localtime(&l_time);

			std::wostringstream l_file_name_stream;
			l_file_name_stream << p_app_name << " ";
			l_file_name_stream << std::put_time(l_local_time, bcL("%Y-%m-%d %H-%M-%S")) << ".txt";
			
			core::bc_path l_path(p_path);
			l_path.create_directory();
			
			l_path.combine(core::bc_path(l_file_name_stream.str().c_str()));
			
			m_file.open_write(l_path.get_string_frame().c_str());
		}

		bc_file_logger::bc_file_logger(bc_file_logger&& p_other) noexcept
			: bci_log_listener(std::move(p_other)),
			m_file(std::move(p_other.m_file)),
			m_buffer_length(p_other.m_buffer_length.load())
		{
		}

		bc_file_logger::~bc_file_logger() = default;

		bc_file_logger& bc_file_logger::operator=(bc_file_logger&& p_other) noexcept
		{
			bci_log_listener::operator=(std::move(p_other));
			m_file = std::move(p_other.m_file);
			m_buffer_length.store(p_other.m_buffer_length.load());
			
			return *this;
		}

		bool bc_file_logger::is_file()
		{
			return true;
		}

		void bc_file_logger::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
		{
			core::bc_estring_frame l_log;
#ifdef BC_UNICODE
			l_log.reserve(std::wcslen(p_log) + 10);
#else
			l_log.reserve(std::strlen(p_log) + 10);
#endif

			switch (p_type)
			{
			case core::bc_log_type::info:
				l_log = bcL("Info: ");
				break;
			case core::bc_log_type::debug:
				l_log = bcL("Debug: ");
				break;
			case core::bc_log_type::warning:
				l_log = bcL("Warning: ");
				break;
			case core::bc_log_type::error:
				l_log = bcL("Error: ");
				break;
			case core::bc_log_type::only_file:
			case core::bc_log_type::all:
			default:
				BC_ASSERT(false);
			}

			l_log.append(p_log);

			core::bc_write_line(m_file, l_log.c_str(), l_log.size());
			
			const auto l_buffer_length = m_buffer_length.fetch_add(l_log.size(), core_platform::bc_memory_order::relaxed);
			if(l_buffer_length >= static_cast<bcSIZE>(core::bc_mem_size::kb))
			{
				m_file.flush();
				m_buffer_length.store(0, core_platform::bc_memory_order::relaxed);
			}
		}
	}
}