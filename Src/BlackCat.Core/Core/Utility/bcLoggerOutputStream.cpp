// [06/19/2021 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcLoggerOutputStream.h"
#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcEnumOperand.h"

namespace black_cat
{
	namespace core
	{
		bc_logger_output_stream::bc_logger_output_stream()
			: m_logger(nullptr),
			m_log_type(bc_log_type::info)
		{
		}

		bc_logger_output_stream& bc_logger_output_stream::start_log(bc_logger& p_logger, bc_log_type p_log_type) noexcept
		{
			m_logger = &p_logger;
			m_log_type = p_log_type;
			return *this;
		}

		bc_logger_output_stream& bc_logger_output_stream::add_only_file_modifier() noexcept
		{
			m_log_type = core::bc_enum::set(m_log_type, bc_log_type::only_file, true);
			return *this;
		}

		void bc_logger_output_stream::flush_log()
		{
			m_logger->log(m_log_type, str().c_str());
			
			clear();
			str(bcL(""));
			seekp(0, std::ios_base::beg);
		}
	}	
}