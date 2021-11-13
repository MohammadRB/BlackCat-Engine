// [11/12/2021 MRB]

#pragma once

#include <sstream>
#include "Core/Container/bcAllocator.h"
#include "Core/File/bcStream.h"

namespace black_cat
{
	namespace core
	{
		template<typename TChar, typename TTraits = std::char_traits<TChar>, typename TAllocator = bc_allocator<TChar>>
		class bc_basic_string_stream : public std::basic_stringstream<TChar, TTraits, TAllocator>, public bci_stream_adapter
		{
		private:
			using base_type = std::basic_stringstream<TChar, TTraits, TAllocator>;
			
		public:
			bc_basic_string_stream();

			bc_basic_string_stream(bc_basic_string_stream&&) noexcept;

			~bc_basic_string_stream() override;

			bc_basic_string_stream& operator=(bc_basic_string_stream&&) noexcept;
			
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

			void swap(bc_basic_string_stream& p_other) noexcept;
		};

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::bc_basic_string_stream() = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::bc_basic_string_stream(bc_basic_string_stream&&) noexcept = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::~bc_basic_string_stream() = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& bc_basic_string_stream<TChar, TTraits, TAllocator>::operator=(bc_basic_string_stream&&) noexcept = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_stream_status bc_basic_string_stream<TChar, TTraits, TAllocator>::get_status() const noexcept
		{
			return bc_stream_status::open;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bool bc_basic_string_stream<TChar, TTraits, TAllocator>::can_read() const noexcept
		{
			return true;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bool bc_basic_string_stream<TChar, TTraits, TAllocator>::can_write() const noexcept
		{
			return true;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bool bc_basic_string_stream<TChar, TTraits, TAllocator>::can_seek() const noexcept
		{
			return true;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bool bc_basic_string_stream<TChar, TTraits, TAllocator>::can_timeout() const noexcept
		{
			return false;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcUINT32 bc_basic_string_stream<TChar, TTraits, TAllocator>::get_timeout() const noexcept
		{
			return std::numeric_limits<bcUINT32>::max();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		void bc_basic_string_stream<TChar, TTraits, TAllocator>::set_timeout(bcUINT32 p_millisecond) noexcept
		{
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcUINT64 bc_basic_string_stream<TChar, TTraits, TAllocator>::get_position() const
		{
			return base_type::tellg();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcUINT64 bc_basic_string_stream<TChar, TTraits, TAllocator>::set_position(bc_stream_seek p_seek_location, bcINT64 p_offset)
		{
			switch (p_seek_location)
			{
			case bc_stream_seek::start:
				base_type::seekg(p_offset, std::ios_base::beg);
				base_type::seekp(p_offset, std::ios_base::beg);
				break;
			case bc_stream_seek::current:
				base_type::seekg(p_offset, std::ios_base::cur);
				base_type::seekp(p_offset, std::ios_base::cur);
				break;
			case bc_stream_seek::end:
				base_type::seekg(p_offset, std::ios_base::end);
				base_type::seekp(p_offset, std::ios_base::end);
				break;
			default: 
				BC_ASSERT(false);
			}

			return get_position();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcUINT64 bc_basic_string_stream<TChar, TTraits, TAllocator>::get_eof_position() const
		{
			auto l_position = get_position();
			base_type::seekg(0, std::ios_base::end);
			auto l_end_position = base_type::tellg();

			base_type::seekg(l_position, std::ios_base::beg);
			base_type::seekp(l_position, std::ios_base::beg);

			return l_end_position;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcSIZE bc_basic_string_stream<TChar, TTraits, TAllocator>::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const
		{
			return base_type::read(static_cast<TChar*>(p_buffer), p_bytes_to_read / sizeof(TChar));
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcSIZE bc_basic_string_stream<TChar, TTraits, TAllocator>::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
		{
			return base_type::write(static_cast<TChar*>(p_buffer), p_bytes_to_write / sizeof(TChar));
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcUINT64 bc_basic_string_stream<TChar, TTraits, TAllocator>::length() const noexcept
		{
			return get_eof_position();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		void bc_basic_string_stream<TChar, TTraits, TAllocator>::flush()
		{
			base_type::flush();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		void bc_basic_string_stream<TChar, TTraits, TAllocator>::close()
		{
			base_type::clear();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_estring bc_basic_string_stream<TChar, TTraits, TAllocator>::get_path() const
		{
			return {};
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_stream_type bc_basic_string_stream<TChar, TTraits, TAllocator>::get_type() const noexcept
		{
			return bc_stream_type::memory;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		void bc_basic_string_stream<TChar, TTraits, TAllocator>::swap(bc_basic_string_stream& p_other) noexcept
		{
			base_type::swap(p_other);
		}

		template<template<typename> typename TAllocator>
		using bc_basic_string_stream_a = bc_basic_string_stream<bcCHAR, std::char_traits<bcCHAR>, TAllocator<bcCHAR>>;

		using bc_string_stream = bc_basic_string_stream_a<bc_allocator>;
		
		using bc_string_stream_frame = bc_basic_string_stream_a<bc_allocator_frame>;

		using bc_string_stream_program = bc_basic_string_stream_a<bc_allocator_program>;

		template<template<typename> typename TAllocator>
		using bc_basic_wstring_stream_a = bc_basic_string_stream<bcWCHAR, std::char_traits<bcWCHAR>, TAllocator<bcWCHAR>>;

		using bc_wstring_stream = bc_basic_wstring_stream_a<bc_allocator>;

		using bc_wstring_stream_frame = bc_basic_wstring_stream_a<bc_allocator_frame>;

		using bc_wstring_stream_program = bc_basic_wstring_stream_a<bc_allocator_program>;
	}
}