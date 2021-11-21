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
			
			bcUINT64 length() const noexcept override;
			
			void flush() override;
			
			void close() override;
			
			bc_estring get_path() const override;
			
			bc_stream_type get_type() const noexcept override;

			void swap(bc_basic_string_stream& p_other) noexcept;

		private:
			bcSIZE read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const override;

			bcSIZE write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write) override;
		};

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::bc_basic_string_stream() = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::bc_basic_string_stream(bc_basic_string_stream&& p_other) noexcept
			: base_type(std::move(p_other))
		{
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>::~bc_basic_string_stream() = default;

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& bc_basic_string_stream<TChar, TTraits, TAllocator>::operator=(bc_basic_string_stream&& p_other) noexcept
		{
			base_type::operator=(p_other);
			return *this;
		}

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
			auto& l_base = const_cast<base_type&>(static_cast<const base_type&>(*this));
			return l_base.tellg();
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
			auto& l_base = const_cast<base_type&>(static_cast<const base_type&>(*this));
			auto l_position = get_position();

			l_base.seekg(0, std::ios_base::end);
			auto l_end_position = l_base.tellg();

			l_base.seekg(l_position, std::ios_base::beg);
			l_base.seekp(l_position, std::ios_base::beg);

			return l_end_position;
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

		template<typename TChar, typename TTraits, typename TAllocator>
		bcSIZE bc_basic_string_stream<TChar, TTraits, TAllocator>::read(bcBYTE* p_buffer, bcSIZE p_bytes_to_read) const
		{
			auto& l_base = const_cast<base_type&>(static_cast<const base_type&>(*this));
			l_base.read(reinterpret_cast<TChar*>(p_buffer), p_bytes_to_read / sizeof(TChar));

			return base_type::gcount();
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bcSIZE bc_basic_string_stream<TChar, TTraits, TAllocator>::write(const bcBYTE* p_buffer, bcSIZE p_bytes_to_write)
		{
			base_type::write(reinterpret_cast<const TChar*>(p_buffer), p_bytes_to_write / sizeof(TChar));
			return p_bytes_to_write; // TODO check for failure in number of bytes written
		}
		
		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bool p_bool)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_bool;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcINT8 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}
		
		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcUINT8 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcINT16 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcUINT16 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcINT32 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcUINT32 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcINT64 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcUINT64 p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcFLOAT p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, bcDOUBLE p_number)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_number;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bcCHAR* p_str)
		{
			std::operator<<(p_stream, p_str);
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bcWCHAR* p_str)
		{
			std::operator<<(p_stream, p_str);
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, std::string_view p_str)
		{
			std::operator<<(p_stream, p_str.data());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, std::wstring_view p_str)
		{
			std::operator<<(p_stream, p_str.data());
			return p_stream;
		}
		
		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_string& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_string_frame& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_string_program& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_wstring& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_wstring_frame& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const bc_wstring_program& p_str)
		{
			std::operator<<(p_stream, p_str.c_str());
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, const void* p_pointer)
		{
			static_cast<std::basic_stringstream<TChar, TTraits, TAllocator>&>(p_stream) << p_pointer;
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, std::ios_base&(*p_func)(std::ios_base&))
		{
			p_func(p_stream);
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, 
			std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ios<bcECHAR, std::char_traits<bcECHAR>>&))
		{
			p_func(p_stream);
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, 
			std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>& (*p_func)(std::basic_ostream<bcECHAR, std::char_traits<bcECHAR>>&))
		{
			p_func(p_stream);
			return p_stream;
		}

		template<typename TChar, typename TTraits, typename TAllocator>
		bc_basic_string_stream<TChar, TTraits, TAllocator>& operator<<(bc_basic_string_stream<TChar, TTraits, TAllocator>& p_stream, 
			bc_basic_string_stream<TChar, TTraits, TAllocator>& (*p_func)(bc_basic_string_stream<TChar, TTraits, TAllocator>&))
		{
			p_func(p_stream);
			return p_stream;
		}
		
		template<template<typename> typename TAllocator>
		using bc_string_stream_a = bc_basic_string_stream<bcCHAR, std::char_traits<bcCHAR>, TAllocator<bcCHAR>>;

		using bc_string_stream = bc_string_stream_a<bc_allocator>;
		
		using bc_string_stream_frame = bc_string_stream_a<bc_allocator_frame>;

		using bc_string_stream_program = bc_string_stream_a<bc_allocator_program>;

		template<template<typename> typename TAllocator>
		using bc_wstring_stream_a = bc_basic_string_stream<bcWCHAR, std::char_traits<bcWCHAR>, TAllocator<bcWCHAR>>;

		using bc_wstring_stream = bc_wstring_stream_a<bc_allocator>;

		using bc_wstring_stream_frame = bc_wstring_stream_a<bc_allocator_frame>;

		using bc_wstring_stream_program = bc_wstring_stream_a<bc_allocator_program>;

#ifdef BC_UNICODE
		using bc_estring_stream = bc_wstring_stream;

		template<template<typename> typename TAllocator>
		using bc_estring_stream_a = bc_wstring_stream_a<TAllocator>;
#else
		using bc_estring_stream = bc_string_stream;

		template<template<typename> typename TAllocator>
		using bc_estring_stream_a = bc_string_stream_a<TAllocator>;
#endif

		using bc_estring_stream_program = bc_estring_stream_a<bc_allocator_program>;

		using bc_estring_stream_frame = bc_estring_stream_a<bc_allocator_frame>;
	}
}