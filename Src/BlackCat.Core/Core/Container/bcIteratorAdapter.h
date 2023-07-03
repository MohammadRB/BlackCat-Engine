// [31/12/2020 MRB]

#pragma once

#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	namespace core
	{
		template<typename T>
		struct _bc_reverse_iterator_or_void
			member_type_or_default
			(
				T,
				reverse_iterator,
				void
			);

		template<typename T>
		struct _bc_const_reverse_iterator_or_void
			member_type_or_default
			(
				T,
				const_reverse_iterator,
				void
			);
		
		template<class TContainer>
		class bc_const_iterator_adapter
		{
		public:
			using container_type = TContainer;
			using value_type = typename container_type::value_type;
			using pointer = typename container_type::pointer;
			using const_pointer = typename container_type::const_pointer;
			using reference = typename container_type::reference;
			using const_reference = typename container_type::const_reference;
			using difference_type = typename container_type::difference_type;
			using size_type = typename container_type::size_type;
			using const_iterator = typename container_type::const_iterator;
			using const_reverse_iterator = typename _bc_const_reverse_iterator_or_void<container_type>::type;

			explicit bc_const_iterator_adapter(const container_type& p_container) noexcept;

			bc_const_iterator_adapter(const bc_const_iterator_adapter& p_other) noexcept = default;
			
			bc_const_iterator_adapter(bc_const_iterator_adapter&& p_other) noexcept = default;

			~bc_const_iterator_adapter() = default;

			bc_const_iterator_adapter& operator=(const bc_const_iterator_adapter& p_other) noexcept = default;
			
			bc_const_iterator_adapter& operator=(bc_const_iterator_adapter&& p_other) noexcept = default;

			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<const_reverse_iterator>>>
			const_reverse_iterator rbegin() const noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<const_reverse_iterator>>>
			const_reverse_iterator crbegin() const noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<const_reverse_iterator>>>
			const_reverse_iterator rend() const noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<const_reverse_iterator>>>
			const_reverse_iterator crend() const noexcept;

			size_type size() const noexcept;

			bool empty() const noexcept;

		protected:
			const container_type* m_container;
		};
		
		template<class TContainer>
		class bc_iterator_adapter : public bc_const_iterator_adapter<TContainer>
		{
		public:
			using container_type = TContainer;
			using iterator = typename container_type::iterator;
			using reverse_iterator = typename _bc_reverse_iterator_or_void<container_type>::type;

			// Make base functions visible
			using bc_const_iterator_adapter<TContainer>::begin;
			using bc_const_iterator_adapter<TContainer>::end;
			using bc_const_iterator_adapter<TContainer>::rbegin;
			using bc_const_iterator_adapter<TContainer>::rend;
		
		public:
			explicit bc_iterator_adapter(container_type& p_container) noexcept;

			bc_iterator_adapter(const bc_iterator_adapter& p_other) noexcept = default;
			
			bc_iterator_adapter(bc_iterator_adapter&& p_other) noexcept = default;

			~bc_iterator_adapter() = default;

			bc_iterator_adapter& operator=(const bc_iterator_adapter& p_other) noexcept = default;
			
			bc_iterator_adapter& operator=(bc_iterator_adapter&& p_other) noexcept = default;

			iterator begin() noexcept;

			iterator end() noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<reverse_iterator>>>
			reverse_iterator rbegin() noexcept;

			template<typename = std::enable_if_t<!std::is_void_v<reverse_iterator>>>
			reverse_iterator rend() noexcept;
		};

		template<class TContainer>
		bc_const_iterator_adapter<TContainer>::bc_const_iterator_adapter(const container_type& p_container) noexcept
			: m_container(&p_container)
		{
		}

		template<class TContainer>
		typename bc_const_iterator_adapter<TContainer>::const_iterator bc_const_iterator_adapter<TContainer>::begin() const noexcept
		{
			return m_container->begin();
		}

		template<class TContainer>
		typename bc_const_iterator_adapter<TContainer>::const_iterator bc_const_iterator_adapter<TContainer>::cbegin() const noexcept
		{
			return m_container->cbegin();
		}

		template<class TContainer>
		typename bc_const_iterator_adapter<TContainer>::const_iterator bc_const_iterator_adapter<TContainer>::end() const noexcept
		{
			return m_container->end();
		}

		template<class TContainer>
		typename bc_const_iterator_adapter<TContainer>::const_iterator bc_const_iterator_adapter<TContainer>::cend() const noexcept
		{
			return m_container->cend();
		}

		template<class TContainer>
		template<typename>
		typename bc_const_iterator_adapter<TContainer>::const_reverse_iterator bc_const_iterator_adapter<TContainer>::rbegin() const noexcept
		{
			return m_container->rbegin();
		}

		template<class TContainer>
		template<typename>
		typename bc_const_iterator_adapter<TContainer>::const_reverse_iterator bc_const_iterator_adapter<TContainer>::crbegin() const noexcept
		{
			return m_container->crbegin();
		}

		template<class TContainer>
		template<typename>
		typename bc_const_iterator_adapter<TContainer>::const_reverse_iterator bc_const_iterator_adapter<TContainer>::rend() const noexcept
		{
			return m_container->rend();
		}

		template<class TContainer>
		template<typename>
		typename bc_const_iterator_adapter<TContainer>::const_reverse_iterator bc_const_iterator_adapter<TContainer>::crend() const noexcept
		{
			return m_container->crend();
		}

		template<class TContainer>
		typename bc_const_iterator_adapter<TContainer>::size_type bc_const_iterator_adapter<TContainer>::size() const noexcept
		{
			return m_container->size();
		}

		template<class TContainer>
		bool bc_const_iterator_adapter<TContainer>::empty() const noexcept
		{
			return m_container->empty();
		}

		template<class TContainer>
		bc_iterator_adapter<TContainer>::bc_iterator_adapter(container_type& p_container) noexcept
			: bc_const_iterator_adapter<TContainer>(p_container)
		{
		}

		template<class TContainer>
		typename bc_iterator_adapter<TContainer>::iterator bc_iterator_adapter<TContainer>::begin() noexcept
		{
			return const_cast<container_type*>(bc_const_iterator_adapter<container_type>::m_container)->begin();
		}
		
		template<class TContainer>
		typename bc_iterator_adapter<TContainer>::iterator bc_iterator_adapter<TContainer>::end() noexcept
		{
			return const_cast<container_type*>(bc_const_iterator_adapter<container_type>::m_container)->end();
		}

		template<class TContainer>
		template<typename>
		typename bc_iterator_adapter<TContainer>::reverse_iterator bc_iterator_adapter<TContainer>::rbegin() noexcept
		{
			return const_cast<container_type*>(bc_const_iterator_adapter<container_type>::m_container)->rbegin();
		}

		template<class TContainer>
		template<typename>
		typename bc_iterator_adapter<TContainer>::reverse_iterator bc_iterator_adapter<TContainer>::rend() noexcept
		{
			return const_cast<container_type*>(bc_const_iterator_adapter<container_type>::m_container)->rend();
		}
	}
}