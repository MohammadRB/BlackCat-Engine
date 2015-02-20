//  [8/23/2013 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"

namespace black_cat
{
	namespace core
	{
		class bc_memory : private core_platform::bc_no_copy
		{
		public:
			using this_type = bc_memory;

		public:
			bc_memory() {}

			bc_memory(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			virtual ~bc_memory() {}

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			bcInline void tag(const bcCHAR* p_tag)
			{
				// In allocators we call this function only in theirs Initialize method, so it is safe /
				std::memcpy(m_tag, p_tag, std::min<bcSIZE>(bcSIZE(s_tag_lenght - 1), strlen(p_tag)));
				*(m_tag + std::min<bcSIZE>(bcSIZE(s_tag_lenght - 1), strlen(p_tag))) = '\0';
			}

			bcInline const bcCHAR* tag() const { return m_tag; };

			bcInline const bc_memory_tracer& tracer() const { return m_tracer; }

			virtual void* alloc(bc_memblock* p_memblock) = 0;

			virtual void free(const void* p_pointer, bc_memblock* p_memblock) = 0;

			virtual bool contain_pointer(const void* p_memory) const = 0;

			virtual void clear() = 0;

		protected:
			bc_memory_tracer m_tracer;

		private:
			static const bcUINT8 s_tag_lenght = 15;
			bcCHAR m_tag[s_tag_lenght];

			void _assign(this_type&& p_other)
			{
				tag(p_other.tag());
				m_tracer = const_cast< bc_memory_tracer& >(p_other.tracer());
			}
		};

		class bc_memory_movable : public bc_memory
		{
		public:
			using this_type = bc_memory_movable;

		public:
			bc_memory_movable() {}

			bc_memory_movable(this_type&& p_other) : bc_memory(std::move(p_other))
			{
			}

			virtual ~bc_memory_movable() {}

			this_type& operator =(this_type&& p_other)
			{
				bc_memory::operator =(std::move(p_other));

				return *this;
			}

#ifdef BC_MEMORY_DEFRAG
			virtual void register_pointer(void** p_pointer) = 0;

			virtual void unregister_pointer(void** p_pointer) = 0;

			virtual void defragment(bcINT32 p_num_defrag) = 0;
#endif

		protected:

		private:
		};
	}
}