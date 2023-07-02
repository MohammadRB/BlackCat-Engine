// [06/06/2021 MRB]

#pragma once

#include "Core/Memory/bcAlloc.h"
#include "Core/File/bcStream.h"
#include "Core/bcException.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class BC_CORE_DLL bc_memory_stream : public bci_stream_adapter
		{
		public:
			bc_memory_stream(bc_alloc_type p_alloc_type = bc_alloc_type::unknown, bcSIZE p_initial_size = 0);

			bc_memory_stream(bc_memory_stream&&) noexcept;

			~bc_memory_stream() override;

			bc_memory_stream& operator=(bc_memory_stream&&) noexcept;

			/**
			 * \brief Return pointer to internal buffer
			 * \return 
			 */
			bcBYTE* get_data() noexcept;

			/**
			 * \brief Return pointer to internal buffer
			 * \return 
			 */
			const bcBYTE* get_data() const noexcept;

			/**
			 * \brief Return pointer within internal buffer which is pointed by current stream position.
			 * \n Undefined behavior if position is outside of buffer boundary
			 * \return 
			 */
			bcBYTE* get_position_data() noexcept;

			/**
			 * \brief Return pointer within internal buffer which is pointed by current stream position.
			 * \n Undefined behavior if position is outside of buffer boundary
			 * \return
			 */
			const bcBYTE* get_position_data() const noexcept;
			
			bc_stream_status get_status() const noexcept override;
			
			bool can_read() const noexcept override;
			
			bool can_write() const noexcept override;
			
			bool can_seek() const noexcept override;
			
			bool can_timeout() const noexcept override;
			
			bcUINT32 get_timeout() const noexcept override;
			
			void set_timeout(bcUINT32 p_millisecond) noexcept override;
			
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
			void _increase_capacity(bcSIZE p_required_size);

			bc_alloc_type m_alloc_type;
			bc_ptr<bcBYTE> m_buffer;
			bcSIZE m_position;
			bcSIZE m_buffer_size;
		};

		inline bc_memory_stream::bc_memory_stream(bc_alloc_type p_alloc_type, bcSIZE p_initial_size)
			: m_alloc_type(p_alloc_type),
			m_buffer(nullptr),
			m_position(0),
			m_buffer_size(0)
		{
			if(p_initial_size)
			{
				_increase_capacity(p_initial_size);
			}
		}

		inline bc_memory_stream::bc_memory_stream(bc_memory_stream&& p_other) noexcept
			: m_alloc_type(p_other.m_alloc_type),
			m_buffer(std::move(p_other.m_buffer)),
			m_position(p_other.m_position),
			m_buffer_size(p_other.m_buffer_size)
		{
			p_other.m_buffer = nullptr;
			p_other.m_position = 0;
			p_other.m_buffer_size = 0;
		}

		inline bc_memory_stream::~bc_memory_stream()
		{
			bc_memory_stream::close();
		}

		inline bc_memory_stream& bc_memory_stream::operator=(bc_memory_stream&& p_other) noexcept
		{
			m_alloc_type = p_other.m_alloc_type;
			m_buffer = p_other.m_buffer;
			m_position = p_other.m_position;
			m_buffer_size = p_other.m_buffer_size;
			p_other.m_buffer = nullptr;
			p_other.m_position = 0;
			p_other.m_buffer_size = 0;

			return *this;
		}

		inline bcBYTE* bc_memory_stream::get_data() noexcept
		{
			return m_buffer;
		}

		inline const bcBYTE* bc_memory_stream::get_data() const noexcept
		{
			return m_buffer;
		}

		inline bcBYTE* bc_memory_stream::get_position_data() noexcept
		{
			BC_ASSERT(m_position <= m_buffer_size);
			return m_buffer + m_position;
		}

		inline const bcBYTE* bc_memory_stream::get_position_data() const noexcept
		{
			return const_cast<bc_memory_stream&>(*this).get_position_data();
		}
		
		inline bc_stream_status bc_memory_stream::get_status() const noexcept
		{
			return m_buffer != nullptr ? bc_stream_status::open : bc_stream_status::close;
		}

		inline bool bc_memory_stream::can_read() const noexcept
		{
			return true;
		}

		inline bool bc_memory_stream::can_write() const noexcept
		{
			return true;
		}

		inline bool bc_memory_stream::can_seek() const noexcept
		{
			return true;
		}

		inline bool bc_memory_stream::can_timeout() const noexcept
		{
			return false;
		}

		inline bcUINT32 bc_memory_stream::get_timeout() const noexcept
		{
			return 0;
		}

		inline void bc_memory_stream::set_timeout(bcUINT32 p_millisecond) noexcept
		{
		}

		inline bcUINT64 bc_memory_stream::get_position() const
		{
			return m_position;
		}

		inline bcUINT64 bc_memory_stream::set_position(bc_stream_seek p_seek_location, bcINT64 p_offset)
		{
			bcINT64 l_new_position;
			switch (p_seek_location)
			{
			case bc_stream_seek::start:
			{
				l_new_position = p_offset;
				break;
			}
			case bc_stream_seek::current:
			{
				l_new_position = m_position + p_offset;
				break;
			}
			case bc_stream_seek::end:
			{				
				l_new_position = m_buffer_size - p_offset;
				break;
			}
			default:
				BC_ASSERT(false);
				return get_position();
			}

			if(l_new_position < 0)
			{
				throw bc_invalid_argument_exception("Provided offset is invalid");
			}
			
			m_position = l_new_position;
			
			return get_position();
		}

		inline bcUINT64 bc_memory_stream::get_eof_position() const
		{
			return m_buffer_size;
		}

		inline bcSIZE bc_memory_stream::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const
		{			
			const auto l_bytes_can_read = static_cast<bcSIZE>(std::max(static_cast<bcINT64>(0), static_cast<bcINT64>(m_buffer_size) - static_cast<bcINT64>(m_position)));
			const auto l_bytes_to_read = std::min(l_bytes_can_read, p_bytes_to_read);
			std::memcpy(p_buffer, m_buffer + m_position, l_bytes_to_read);

			return l_bytes_to_read;
		}

		inline bcSIZE bc_memory_stream::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
		{
			const auto l_bytes_can_write = static_cast<bcSIZE>(std::max(static_cast<bcINT64>(0), static_cast<bcINT64>(m_buffer_size) - static_cast<bcINT64>(m_position)));
			if(l_bytes_can_write < p_bytes_to_write)
			{
				_increase_capacity(p_bytes_to_write);
			}
			
			std::memcpy(m_buffer + m_position, p_buffer, p_bytes_to_write);
			m_position += p_bytes_to_write;

			return p_bytes_to_write;
		}

		inline bcUINT64 bc_memory_stream::length() const noexcept
		{
			return m_buffer_size;
		}

		inline void bc_memory_stream::flush()
		{
		}

		inline void bc_memory_stream::close()
		{
			auto* l_pointer = m_buffer.release();
			BC_ALIGNED_FREE(l_pointer);
			m_position = 0;
			m_buffer_size = 0;
		}

		inline bc_estring bc_memory_stream::get_path() const
		{
			return {};
		}

		inline bc_stream_type bc_memory_stream::get_type() const noexcept
		{
			return bc_stream_type::memory;
		}
	}
}