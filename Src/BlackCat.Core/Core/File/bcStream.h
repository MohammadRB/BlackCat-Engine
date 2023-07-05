// [05/04/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"

namespace black_cat::core
{
	enum class bc_stream_status
	{
		open,	// Stream is open and ready
		close,	// Stream is closed
		error	// Stream isn't open and error occurred during file opening
	};

	enum class bc_stream_seek
	{
		start,
		current,
		end
	};

	enum class bc_stream_type
	{
		file,
		memory,
		network
	};

	class bci_stream_adapter
	{
	public:
		bci_stream_adapter() = default;

		virtual ~bci_stream_adapter() = default;

		virtual bc_stream_status get_status() const noexcept = 0;

		virtual bool can_read() const noexcept = 0;

		virtual bool can_write() const noexcept = 0;

		virtual bool can_seek() const noexcept = 0;

		virtual bool can_timeout() const noexcept = 0;

		virtual bcUINT32 get_timeout() const noexcept = 0;

		virtual void set_timeout(bcUINT32 p_millisecond) noexcept = 0;

		virtual bcUINT64 get_position() const = 0;

		/**
			 * \brief Move stream pointer by offset(bytes) and return new location from beginning of stream.
			 * \n Implementations should allow position to exceed current stream capacity.
			 * \param p_seek_location 
			 * \param p_offset 
			 * \return 
			 */
		virtual bcUINT64 set_position(bc_stream_seek p_seek_location, bcINT64 p_offset) = 0;

		/**
			 * \brief Get end of stream pointer
			 * \return 
			 */
		virtual bcUINT64 get_eof_position() const = 0;

		/**
			 * \brief Return number of bytes which has been read
			 * \param p_buffer
			 * \param p_bytes_to_read
			 * \return
			 */
		virtual bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const = 0;

		/**
			 * \brief Return number of bytes which has been written
			 * \param p_buffer
			 * \param p_bytes_to_write
			 * \return
			 */
		virtual bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write) = 0;

		virtual bcUINT64 length() const noexcept = 0;

		virtual void flush() = 0;

		virtual void close() = 0;

		virtual bc_estring get_path() const = 0;

		virtual bc_stream_type get_type() const noexcept = 0;
	};

	class bc_stream : platform::bc_no_copy
	{
	public:
		bc_stream();
			
		/**
			 * \brief Initialize stream with an actual stream object
			 * \tparam TStreamAdapter 
			 * \param p_stream 
			 */
		template
		< 
			class TStreamAdapter,
			typename = std::enable_if_t
			<
				!std::is_lvalue_reference_v<TStreamAdapter> 
				&&
				!std::is_same_v<std::decay_t<TStreamAdapter>, bc_stream> 
			> 
		>
		explicit bc_stream(TStreamAdapter&& p_stream);

		bc_stream(bc_stream&& p_other) noexcept;

		~bc_stream();

		bc_stream& operator=(bc_stream&& p_other) noexcept;

		bc_stream_status get_status() const noexcept;

		bool is_open() const noexcept;

		bool is_close() const noexcept;

		bool can_read() const noexcept;

		bool can_write() const noexcept;

		bool can_seek() const noexcept;

		bool can_timeout() const noexcept;

		bcUINT32 get_timeout() const noexcept;

		void set_timeout(bcUINT32 p_millisecond);

		/**
			 * \brief Get stream pointer from beginning of stream
			 * \return 
			 */
		bcUINT64 get_position() const;

		/**
			 * \brief Move stream pointer by offset(bytes) and return new location from beginning of stream
			 * \param p_seek_location 
			 * \param p_offset 
			 * \return 
			 */
		bcUINT64 set_position(bc_stream_seek p_seek_location, bcINT64 p_offset);

		/**
			 * \brief Return number of bytes which has been read
			 * \param p_buffer 
			 * \param p_bytes_to_read 
			 * \return 
			 */
		bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read);

		/**
			 * \brief Return number of bytes which has been written
			 * \param p_buffer 
			 * \param p_bytes_to_write 
			 * \return 
			 */
		bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write);

		bcUINT64 length() const noexcept;

		void flush();

		void close();

		bc_estring get_path() const;

		bc_stream_type get_type() const noexcept;

	private:
		bc_unique_ptr<bci_stream_adapter> m_stream;
	};

	inline bc_stream::bc_stream()
		: m_stream(nullptr)
	{
	}
		
	template< class TStreamAdapter, typename >
	bc_stream::bc_stream(TStreamAdapter&& p_stream)
	{
		using stream_adapter_t = std::remove_reference_t< TStreamAdapter >;
		static_assert(std::is_base_of< bci_stream_adapter, stream_adapter_t >::value, "Stream class must inherite from bc_istream_adapter");

		m_stream = core::bc_make_unique< stream_adapter_t >(bc_alloc_type::unknown_movable, std::move(p_stream));
	}

	inline bc_stream::bc_stream(bc_stream&& p_other) noexcept = default;

	inline bc_stream::~bc_stream()
	{
		if(get_status() == bc_stream_status::open)
		{
			m_stream->close();
		}
	}

	inline bc_stream& bc_stream::operator=(bc_stream&& p_other) noexcept = default;

	inline bc_stream_status bc_stream::get_status() const noexcept
	{
		return m_stream ? m_stream->get_status() : bc_stream_status::error;
	}

	inline bool bc_stream::is_open() const noexcept
	{
		return get_status() == bc_stream_status::open;
	}

	inline bool bc_stream::is_close() const noexcept
	{
		return get_status() == bc_stream_status::close;
	}

	inline bool bc_stream::can_read() const noexcept
	{
		return m_stream->can_read();
	}

	inline bool bc_stream::can_write() const noexcept
	{
		return m_stream->can_write();
	}

	inline bool bc_stream::can_seek() const noexcept
	{
		return m_stream->can_seek();
	}

	inline bool bc_stream::can_timeout() const noexcept
	{
		return m_stream->can_timeout();
	}

	inline bcUINT32 bc_stream::get_timeout() const noexcept
	{
		return m_stream->get_timeout();
	}

	inline void bc_stream::set_timeout(bcUINT32 p_millisecond)
	{
		return m_stream->set_timeout(p_millisecond);
	}

	inline bcUINT64 bc_stream::get_position() const
	{
		return m_stream->get_position();
	}

	inline bcUINT64 bc_stream::set_position(bc_stream_seek p_seek_location, bcINT64 p_offset)
	{
		return m_stream->set_position(p_seek_location, p_offset);
	}

	inline bcSIZE bc_stream::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read)
	{
		return m_stream->read(p_buffer, p_bytes_to_read);
	}

	inline bcSIZE bc_stream::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
	{
		return m_stream->write(p_buffer, p_bytes_to_write);
	}

	inline bcUINT64 bc_stream::length() const noexcept
	{
		return m_stream->length();
	}

	inline void bc_stream::flush()
	{
		return m_stream->flush();
	}

	inline void bc_stream::close()
	{
		return m_stream->close();
	}

	inline bc_estring bc_stream::get_path() const
	{
		return m_stream->get_path();
	}

	inline bc_stream_type bc_stream::get_type() const noexcept
	{
		return m_stream->get_type();
	}

	/**
		 * \brief Read a char from stream if stream is readable and stream position isn't at end
		 * \param p_stream 
		 * \param p_char 
		 * \return 
		 */
	bool BC_CORE_DLL bc_read_char(bci_stream_adapter& p_stream, bcCHAR* p_char);
		
	/**
		 * \brief Read a wchar from stream if stream is readable and stream position isn't at end
		 * \param p_stream 
		 * \param p_char 
		 * \return 
		 */
	bool BC_CORE_DLL bc_read_char(bci_stream_adapter& p_stream, bcWCHAR* p_char);

	/**
		 * \brief Read a line from stream if stream is readable and stream position isn't at end
		 * \param p_stream 
		 * \param p_line 
		 * \return 
		 */
	bool BC_CORE_DLL bc_read_line(bci_stream_adapter& p_stream, bc_string_frame& p_line);

	/**
		 * \brief Read a line from stream if stream is readable and stream position isn't at end
		 * \param p_stream 
		 * \param p_line 
		 * \return 
		 */
	bool BC_CORE_DLL bc_read_line(bci_stream_adapter& p_stream, bc_wstring_frame& p_line);

	/**
		 * \brief Read stream to the end if stream is readable and stream position isn't at end
		 * \param p_stream
		 * \param p_content
		 * \return
		 */
	bool BC_CORE_DLL bc_read_all_lines(bci_stream_adapter& p_stream, bc_string_frame& p_content);

	/**
		 * \brief Read stream to the end if stream is readable and stream position isn't at end
		 * \param p_stream
		 * \param p_content
		 * \return
		 */
	bool BC_CORE_DLL bc_read_all_lines(bci_stream_adapter& p_stream, bc_wstring_frame& p_content);

	/**
		 * \brief Write a char to stream if stream is writable and stream position isn't at end
		 * \param p_stream
		 * \param p_char
		 * \return
		 */
	bool BC_CORE_DLL bc_write_char(bci_stream_adapter& p_stream, const bcCHAR* p_char);

	/**
		 * \brief Write a char to stream if stream is writable and stream position isn't at end
		 * \param p_stream
		 * \param p_char
		 * \return
		 */
	bool BC_CORE_DLL bc_write_char(bci_stream_adapter& p_stream, const bcWCHAR* p_char);

	/**
		 * \brief Write a line to stream if stream is readable and stream position isn't at end
		 * \param p_stream
		 * \param p_line
		 * \param p_length
		 * \return
		 */
	bool BC_CORE_DLL bc_write_line(bci_stream_adapter& p_stream, const bcCHAR* p_line, bcSIZE p_length);

	/**
		 * \brief Write a line to stream if stream is readable and stream position isn't at end
		 * \param p_stream
		 * \param p_line
		 * \param p_length
		 * \return
		 */
	bool BC_CORE_DLL bc_write_line(bci_stream_adapter& p_stream, const bcWCHAR* p_line, bcSIZE p_length);
}
