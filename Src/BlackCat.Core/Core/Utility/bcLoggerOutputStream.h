// [06/19/2021 MRB]

#pragma once

#include <sstream>
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcString.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_logger;

		enum class bc_log_type : bcUBYTE
		{
			info = 1,
			debug = 2,
			warning = 4,
			error = 8,
			all = info | debug | warning | error
		};
		
		class BC_CORE_DLL bc_logger_output_stream : public std::basic_ostringstream<bcECHAR, std::char_traits<bcECHAR>, bc_allocator<bcECHAR>>
		{
			using output_stream = std::basic_ostringstream<bcECHAR, std::char_traits<bcECHAR>, bc_allocator<bcECHAR>>;
			
		public:
			bc_logger_output_stream();

			bc_logger_output_stream(const bc_logger_output_stream&) = delete;
			
			bc_logger_output_stream(bc_logger_output_stream&&) = delete;

			~bc_logger_output_stream() override = default;

			bc_logger_output_stream& operator=(bc_logger_output_stream&&) = delete;
			
			bc_logger_output_stream& operator=(const bc_logger_output_stream&) = delete;

			bc_logger_output_stream& start_log(bc_logger& p_logger, bc_log_type p_log_type);

			bc_logger_output_stream& operator<<(bool p_bool);
			
			bc_logger_output_stream& operator<<(bcINT8 p_number);

			bc_logger_output_stream& operator<<(bcUINT8 p_number);

			bc_logger_output_stream& operator<<(bcINT16 p_number);

			bc_logger_output_stream& operator<<(bcUINT16 p_number);

			bc_logger_output_stream& operator<<(bcINT32 p_number);

			bc_logger_output_stream& operator<<(bcUINT32 p_number);

			bc_logger_output_stream& operator<<(bcINT64 p_number);

			bc_logger_output_stream& operator<<(bcUINT64 p_number);

			bc_logger_output_stream& operator<<(bcFLOAT p_number);

			bc_logger_output_stream& operator<<(bcDOUBLE p_number);

			bc_logger_output_stream& operator<<(const bcCHAR* p_str);
			
			bc_logger_output_stream& operator<<(const bcWCHAR* p_str);

			bc_logger_output_stream& operator<<(const bc_string& p_str);
			
			bc_logger_output_stream& operator<<(const bc_string_frame& p_str);
			
			bc_logger_output_stream& operator<<(const bc_string_program& p_str);

			bc_logger_output_stream& operator<<(const bc_wstring& p_str);

			bc_logger_output_stream& operator<<(const bc_wstring_frame& p_str);

			bc_logger_output_stream& operator<<(const bc_wstring_program& p_str);

			bc_logger_output_stream& operator<<(const void* p_pointer);

			bc_logger_output_stream& operator<<(std::ios_base& (*p_func)(std::ios_base&));
			
			bc_logger_output_stream& operator<<(std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>&));
			
			bc_logger_output_stream& operator<<(std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>&));
			
			bc_logger_output_stream& operator<<(bc_logger_output_stream& (*p_func)(bc_logger_output_stream&));

			void flush_log();
		
		private:
			bc_logger* m_logger;
			bc_log_type m_log_type;
		};

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bool p_bool)
		{
			output_stream::operator<<(p_bool);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcINT8 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcUINT8 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcINT16 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcUINT16 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcINT32 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcUINT32 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcINT64 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcUINT64 p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcFLOAT p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bcDOUBLE p_number)
		{
			output_stream::operator<<(p_number);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bcCHAR* p_str)
		{
			std::operator<<(*this, p_str);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bcWCHAR* p_str)
		{
			std::operator<<(*this, p_str);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_string& p_str)
		{
			output_stream::operator<<(p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_string_frame& p_str)
		{
			std::operator<<(*this, p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_string_program& p_str)
		{
			std::operator<<(*this, p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_wstring& p_str)
		{
			std::operator<<(*this, p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_wstring_frame& p_str)
		{
			std::operator<<(*this, p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const bc_wstring_program& p_str)
		{
			std::operator<<(*this, p_str.c_str());
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(const void* p_pointer)
		{
			output_stream::operator<<(p_pointer);
			return *this;
		}
		
		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(std::ios_base& (*p_func)(std::ios_base&))
		{
			p_func(*this);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>&))
		{
			p_func(*this);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>&))
		{
			p_func(*this);
			return *this;
		}

		inline bc_logger_output_stream& bc_logger_output_stream::operator<<(bc_logger_output_stream& (*p_func)(bc_logger_output_stream&))
		{
			p_func(*this);
			return *this;
		}

		inline bc_logger_output_stream& bc_lend(bc_logger_output_stream& p_output_stream)
		{
			p_output_stream.flush_log();
			return p_output_stream;
		}
	}
}