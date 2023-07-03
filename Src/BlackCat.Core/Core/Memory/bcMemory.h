//  [23/8/2013 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Memory/bcMemoryTracer.h"
#include "Core/Utility/bcDelegate.h"

namespace black_cat
{
	namespace core
	{
		class bci_memory : private platform::bc_no_copy
		{
		public:
			using this_type = bci_memory;

		public:
			bci_memory();

			bci_memory(this_type&& p_other) noexcept;

			virtual ~bci_memory();

			this_type& operator =(this_type&& p_other) noexcept;

			void tag(const bcCHAR* p_tag);

			const bcCHAR* tag() const noexcept;

			const bc_memory_tracer& tracer() const noexcept;

			virtual void* alloc(bc_memblock* p_memblock) noexcept = 0;

			virtual void free(void* p_pointer, bc_memblock* p_memblock) noexcept = 0;

			virtual bool contain_pointer(void* p_memory) const noexcept = 0;

			virtual void clear() noexcept = 0;

		protected:
			bc_memory_tracer m_tracer;

		private:
			static constexpr bcUINT8 s_tag_length = 15;
			bcCHAR m_tag[s_tag_length];

			void _assign(this_type&& p_other);
		};

		class bci_memory_movable : public bci_memory
		{
		public:
			using this_type = bci_memory_movable;
			using defrag_callback = bc_delegate<void(void*, void*)>;

		public:
			bci_memory_movable();

			bci_memory_movable(this_type&& p_other) noexcept;

			~bci_memory_movable() override;

			this_type& operator =(this_type&& p_other) noexcept;

#ifdef BC_MEMORY_DEFRAG
			virtual void register_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept = 0;

			virtual void unregister_pointer(void** p_pointer, bc_memblock* p_memblock) noexcept = 0;

			virtual void defrag(bcINT32 p_num_defrag, defrag_callback p_defrag_callback) noexcept = 0;
#endif
		};

		inline bci_memory::bci_memory() = default;

		inline bci_memory::bci_memory(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		inline bci_memory::~bci_memory() = default;

		inline bci_memory::this_type& bci_memory::operator=(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));

			return *this;
		}

		inline void bci_memory::tag(const bcCHAR* p_tag)
		{
			// In allocators we call this function only in theirs Initialize method, so it is safe /
			std::memcpy(m_tag, p_tag, std::min<bcSIZE>(static_cast<bcSIZE>(s_tag_length - 1), strlen(p_tag)));
			*(m_tag + std::min<bcSIZE>(static_cast<bcSIZE>(s_tag_length - 1), strlen(p_tag))) = '\0';
		}

		inline const bcCHAR* bci_memory::tag() const noexcept
		{
			return m_tag;
		}

		inline const bc_memory_tracer& bci_memory::tracer() const noexcept
		{
			return m_tracer;
		}

		inline void bci_memory::_assign(this_type&& p_other)
		{
			tag(p_other.tag());
			m_tracer = p_other.tracer();
		}

		inline bci_memory_movable::bci_memory_movable() = default;

		inline bci_memory_movable::bci_memory_movable(this_type&& p_other) noexcept: bci_memory(std::move(p_other))
		{
		}

		inline bci_memory_movable::~bci_memory_movable() = default;

		inline bci_memory_movable::this_type& bci_memory_movable::operator=(this_type&& p_other) noexcept
		{
			bci_memory::operator =(std::move(p_other));

			return *this;
		}
	}
}