// [05/04/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/File/bcFile.h"
#include "Core/File/bcStream.h"

namespace black_cat::core
{
	class bc_file_stream : 
			public bci_stream_adapter,
			public platform::bc_no_copy
	{
	public:
		bc_file_stream();

		bc_file_stream(bc_file_stream&& p_other) noexcept(std::is_nothrow_move_constructible<platform::bc_file>::value);

		~bc_file_stream() override;

		bc_file_stream& operator=(bc_file_stream&& p_other) noexcept(std::is_nothrow_move_assignable<platform::bc_file>::value);

		const platform::bc_file& get_file() const noexcept;

		bc_stream_status get_status() const noexcept override;

		bool can_read() const noexcept override;

		bool can_write() const noexcept override;

		bool can_seek() const noexcept override;

		bool can_timeout() const noexcept override;

		bcUINT32 get_timeout() const noexcept override;

		void set_timeout(bcUINT32 p_millisecond) noexcept override;
		
		/**
			 * \brief This function does not throw exception but if it fail any subsequence calls to object will throw exception
			 * \param p_file 
			 * \param p_mode 
			 * \param p_access 
			 * \param p_sharing 
			 * \return 
			 */
		bool open(bc_estring_view p_file, platform::bc_file_mode p_mode, platform::bc_file_access p_access, platform::bc_file_sharing p_sharing) noexcept;

		bool open(bc_estring_view p_file, platform::bc_file_mode p_mode, platform::bc_file_access p_access) noexcept;
			
		/**
			 * \brief Open an existing file for reading
			 * \param p_file 
			 * \return 
			 */
		bool open_read(bc_estring_view p_file) noexcept;

		/**
			 * \brief Open an existing file or create a new file for writing
			 * \param p_file 
			 * \return 
			 */
		bool open_write(bc_estring_view p_file) noexcept;

		bcUINT64 get_position() const override;

		bcUINT64 set_position(bc_stream_seek p_seek_location, bcINT64 p_offset) override;

		bcUINT64 get_eof_position() const override;

		bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const override;

		bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write) override;

		bcUINT64 length() const noexcept override;

		void flush() override;

		void close() override;

		bc_estring get_path() const override;

		bc_stream_type get_type() const noexcept override;

	private:
		platform::bc_file m_file;
		platform::bc_file_access m_access;
		bc_estring m_path;
	};

	inline bc_file_stream::bc_file_stream() = default;

	inline bc_file_stream::bc_file_stream(bc_file_stream&& p_other) noexcept (std::is_nothrow_move_constructible_v<platform::bc_file>) = default;

	inline bc_file_stream::~bc_file_stream()
	{
		if (get_status() == bc_stream_status::open)
		{
			close();
		}
	}

	inline bc_file_stream& bc_file_stream::operator=(bc_file_stream&& p_other) noexcept (std::is_nothrow_move_constructible_v<platform::bc_file>) = default;

	inline const platform::bc_file& bc_file_stream::get_file() const noexcept
	{
		return m_file;
	}

	inline bc_stream_status bc_file_stream::get_status() const noexcept
	{
		bc_stream_status l_stream_status = bc_stream_status::error;
		platform::bc_file_status l_file_status = m_file.get_status();

		switch (l_file_status)
		{
		case platform::bc_file_status::open:
			l_stream_status = bc_stream_status::open;
			break;
		case platform::bc_file_status::close:
			l_stream_status = bc_stream_status::close;
			break;
		case platform::bc_file_status::error:
			l_stream_status = bc_stream_status::error;
			break;
		}

		return l_stream_status;
	}

	inline bool bc_file_stream::can_read() const noexcept
	{
		return m_access == platform::bc_file_access::read || m_access == platform::bc_file_access::read_write;
	}

	inline bool bc_file_stream::can_write() const noexcept
	{
		return m_access == platform::bc_file_access::write || m_access == platform::bc_file_access::read_write;
	}

	inline bool bc_file_stream::can_seek() const noexcept
	{
		return true;
	}

	inline bool bc_file_stream::can_timeout() const noexcept
	{
		return false;
	}

	inline bcUINT32 bc_file_stream::get_timeout() const noexcept
	{
		return 0;
	}

	inline void bc_file_stream::set_timeout(bcUINT32 p_millisecond) noexcept
	{
	}

	inline bool bc_file_stream::open(bc_estring_view p_file, platform::bc_file_mode p_mode, platform::bc_file_access p_access, platform::bc_file_sharing p_sharing) noexcept
	{
		const bool l_result = m_file.open(p_file.data(), p_mode, p_access, p_sharing);
			
		if (l_result)
		{
			m_path.assign(p_file);
			m_access = p_access;
		}

		return l_result;
	}

	inline bool bc_file_stream::open(bc_estring_view p_file, platform::bc_file_mode p_mode, platform::bc_file_access p_access) noexcept
	{
		const bool l_result = m_file.open(p_file.data(), p_mode, p_access);

		if (l_result)
		{
			m_path.assign(p_file);
			m_access = p_access;
		}

		return l_result;
	}

	inline bool bc_file_stream::open_read(bc_estring_view p_file) noexcept
	{
		const bool l_result = m_file.open_read(p_file.data());

		if (l_result)
		{
			m_path.assign(p_file);
			m_access = platform::bc_file_access::read;
		}

		return l_result;
	}

	inline bool bc_file_stream::open_write(bc_estring_view p_file) noexcept
	{
		const bool l_result = m_file.open_write(p_file.data());

		if (l_result)
		{
			m_path.assign(p_file);
			m_access = platform::bc_file_access::write;
		}

		return l_result;
	}

	inline bcUINT64 bc_file_stream::get_position() const
	{
		return m_file.get_pointer();
	}

	inline bcUINT64 bc_file_stream::set_position(bc_stream_seek p_seek_location, bcINT64 p_offset)
	{
		platform::bc_file_seek l_file_seek = platform::bc_file_seek::current;

		switch (p_seek_location)
		{
		case bc_stream_seek::start:
			l_file_seek = platform::bc_file_seek::start;
			break;
		case bc_stream_seek::current:
			l_file_seek = platform::bc_file_seek::current;
			break;
		case bc_stream_seek::end:
			l_file_seek = platform::bc_file_seek::end;
			break;
		}

		return m_file.set_pointer(l_file_seek, p_offset);
	}

	inline bcUINT64 bc_file_stream::get_eof_position() const
	{
		return m_file.get_eof_pointer();
	}

	inline bcSIZE bc_file_stream::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const
	{
		return m_file.read(p_buffer, p_bytes_to_read);
	}

	inline bcSIZE bc_file_stream::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
	{
		return m_file.write(p_buffer, p_bytes_to_write);
	}

	inline bcUINT64 bc_file_stream::length() const noexcept
	{
		return m_file.size();
	}

	inline void bc_file_stream::flush()
	{
		return m_file.flush();
	}

	inline void bc_file_stream::close()
	{
		m_file.close();
	}

	inline bc_estring bc_file_stream::get_path() const
	{
		return m_path;
	}

	inline bc_stream_type bc_file_stream::get_type() const noexcept
	{
		return bc_stream_type::file;
	}
}
