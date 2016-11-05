// [04/05/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/CorePCH.h"
#include "Core/bcExport.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		enum class bc_stream_status
		{
			open,	// Stream is open and ready
			close,	// Stream is closed
			error	// Stream isn't open and error occured during file opening
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

		class bc_istream_adapter
		{
		public:
			bc_istream_adapter() = default;

			virtual ~bc_istream_adapter() = default;

			virtual bc_stream_status get_status() const noexcept = 0;

			virtual bool can_read() const noexcept = 0;

			virtual bool can_write() const noexcept = 0;

			virtual bool can_seek() const noexcept = 0;

			virtual bool can_timeout() const noexcept = 0;

			virtual bcUINT32 get_timeout() const noexcept = 0;

			virtual void set_timeout(bcUINT32 p_milisecond) const noexcept = 0;

			virtual bcUINT64 get_position() const = 0;

			virtual bcUINT64 set_position(bc_stream_seek p_seek_location, bcINT64 p_offset) = 0;

			virtual bcUINT64 get_eof_position() const = 0;

			virtual bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) = 0;

			virtual bcSIZE write(bcBYTE* p_buffer, bcSIZE p_bytes_to_write) = 0;

			virtual bcUINT64 length() const noexcept = 0;

			virtual void flush() = 0;

			virtual void close() = 0;

			virtual bc_estring get_path() const = 0;

			virtual bc_stream_type get_type() const noexcept = 0;
		};

		class bc_stream : core_platform::bc_no_copy
		{
		public:
			// Initialize stream with an actual stream object
			template< class TStreamAdapter >
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

			void set_timeout(bcUINT32 p_milisecond);

			// Get stream pointer from beginig of stream
			bcUINT64 get_position() const;

			// Move stream pointer by offset(bytes) and return new location from begining of stream
			bcUINT64 set_position(bc_stream_seek p_seek_location, bcINT64 p_offset);

			// Return number of bytes that has been read
			bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read);

			// Return number of bytes that has been written
			bcSIZE write(bcBYTE* p_buffer, bcSIZE p_bytes_to_write);

			bcUINT64 length() const noexcept;

			void flush();

			void close();

			bc_estring get_path() const;

			bc_stream_type get_type() const noexcept;

		protected:

		private:
			bc_unique_ptr< bc_istream_adapter > m_stream;
		};

		template< class TStreamAdapter >
		bc_stream::bc_stream(TStreamAdapter&& p_stream)
		{
			using stream_adapter_t = typename std::remove_reference< TStreamAdapter >::type;
			static_assert(std::is_base_of< bc_istream_adapter, stream_adapter_t >::value, "Stream class must inherite from bc_istream_adapter");

			m_stream = core::bc_make_unique< stream_adapter_t >(bc_alloc_type::unknown_movale, std::move(p_stream));
		}

		inline bc_stream::bc_stream(bc_stream&& p_other) noexcept = default;

		inline bc_stream::~bc_stream() = default;

		inline bc_stream& bc_stream::operator=(bc_stream&& p_other) noexcept = default;

		inline bc_stream_status bc_stream::get_status() const noexcept
		{
			return m_stream->get_status();
		}

		inline bool bc_stream::is_open() const noexcept
		{
			return m_stream->get_status() == bc_stream_status::open;
		}

		inline bool bc_stream::is_close() const noexcept
		{
			return m_stream->get_status() == bc_stream_status::close;
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

		inline void bc_stream::set_timeout(bcUINT32 p_milisecond)
		{
			return m_stream->set_timeout(p_milisecond);
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

		inline bcSIZE bc_stream::write(bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
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

		// Read a char from stream if stream is readable and stream position isn't at end
		bool BC_CORE_DLL bc_get_char(bc_istream_adapter& p_stream, bcCHAR* p_char);

		// Read a wchar from stream if stream is readable and stream position isn't at end
		bool BC_CORE_DLL bc_get_wchar(bc_istream_adapter& p_stream, bcWCHAR* p_char);

		// Read a line from stream and replace in p_line string if stream is readable and stream position isn't at end
		bool BC_CORE_DLL bc_get_line(bc_istream_adapter& p_stream, bc_string_frame* p_line);

		// Read a line from stream and replace in p_line string if stream is readable and stream position isn't at end
		bool BC_CORE_DLL bc_get_wline(bc_istream_adapter& p_stream, bc_wstring_frame* p_line);
	}
}
