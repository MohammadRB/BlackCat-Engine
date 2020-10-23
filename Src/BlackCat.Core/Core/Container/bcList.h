// [12/30/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"

namespace black_cat
{
	namespace core
	{
		template < typename T, class TAllocator >
		class bc_list_base : public bc_container< T, TAllocator >
		{
		public:
			using this_type = bc_list_base;
			using base_type = bc_container< T, TAllocator >;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer_type = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;

		protected:
			struct node;
			using node_type = node;
			using node_pointer = typename std::pointer_traits< pointer_type >::template rebind< node_type >;
			using internal_allocator_type = typename bc_allocator_traits< allocator_type >::template rebind_alloc< node_type >::other;

		public:
			bc_list_base(const allocator_type& p_allocator = allocator_type())
				: m_head(nullptr),
				m_allocator(p_allocator)
			{
			}

			~bc_list_base() = default;

			struct node : public bc_container_node< value_type >
			{
			public:
				node()
					: m_next(nullptr),
					m_prev(nullptr)
				{
				}

				node(const value_type& p_value) noexcept(std::is_nothrow_copy_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(p_value),
					m_next(nullptr),
					m_prev(nullptr)
				{
				}

				node(value_type&& p_value) noexcept(std::is_nothrow_move_constructible<bc_container_node<value_type>>::value)
					: bc_container_node(std::move(p_value)),
					m_next(nullptr),
					m_prev(nullptr)
				{
				}

				node(const node& p_other)
					: bc_container_node(p_other.m_value),
					m_next(p_other.m_next),
					m_prev(p_other.m_prev)
				{
				}

				node(node&& p_other)
					: bc_container_node(std::move(p_other.m_value)),
					m_next(p_other.m_next),
					m_prev(p_other.m_prev)
				{
					p_other.m_next = nullptr;
					p_other.m_prev = nullptr;
				}

				template< typename ...TArgs >
				node(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<bc_container_node<value_type>, TArgs...>::value)
					: bc_container_node(std::forward<TArgs>(p_args)...),
					m_next(nullptr),
					m_prev(nullptr)
				{
				}

				~node()
				{
				}

				node_pointer m_next;
				node_pointer m_prev;
			};

		protected:
			bool iterator_validate(const node_type* p_node) const noexcept
			{
				return p_node;
			}

			bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept
			{
				return p_first == p_second ? 0 : p_first > p_second ? 1 : -1;
			}

			value_type* iterator_dereference(node_type* p_node) const noexcept
			{
				return std::addressof(p_node->m_value);
			}

			node_type* iterator_increment(node_type* p_node) const noexcept
			{
				if(p_node->m_next == m_head) // Don't let iterators iterate circularly
				{
					return nullptr;
				}
				return p_node->m_next;
			}

			node_type* iterator_decrement(node_type* p_node) const noexcept
			{
				if (p_node == m_head) // Don't let iterators iterate circularly
				{
					return nullptr;
				}
				return p_node->m_prev;
			}

			void iterator_swap(node_type** p_first, node_type** p_second) const noexcept
			{
				std::swap(*p_first, *p_second);
			}

			template< typename ...TArgs >
			node_type* _new_head(TArgs&&... p_args)
			{
				m_head = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
				bc_allocator_traits< internal_allocator_type >::construct(m_allocator, m_head, std::forward<TArgs>(p_args)...);

				m_head->m_next = m_head;
				m_head->m_prev = m_head;

				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head);
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head->m_next);
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head->m_prev);

				return m_head;
			}

			node_type* _new_head(node_type* p_new_node)
			{
				m_head = p_new_node;
				m_head->m_next = m_head;
				m_head->m_prev = m_head;

				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head);
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head->m_next);
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head->m_prev);

				return m_head;
			}

			node_type* _swap_head(node_type* p_new_head)
			{
				node_type* l_current_head = m_head;
				
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &m_head);
				m_head = p_new_head;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &m_head);

				return l_current_head;
			}
			
			template< typename ...TArgs >
			node_type* _new_node(node_type* p_prev, node_type* p_next, size_type p_count, TArgs&&... p_args)
			{
				bcAssert(p_prev == nullptr || p_next == nullptr);

				const bool l_swap_head = p_next == m_head;
				
				if(!p_prev && p_next)
				{
					p_prev = p_next->m_prev;
				}

				if(!p_next && p_prev)
				{
					p_next = p_prev->m_next;
				}

				node_type* l_first_inserted = nullptr;
				size_type l_count = 0;

				if(!p_prev && !p_next && !m_head)
				{
					l_first_inserted = p_prev = p_next = _new_head(std::forward<TArgs>(p_args)...);
					++l_count;
				}
				else if(!p_prev && !p_next && m_head)
				{
					p_prev = m_head->m_prev;
					p_next = p_prev->m_next;
				}
				
				for (; l_count < p_count; ++l_count)
				{
					node_type* l_new_node = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
					bc_allocator_traits< internal_allocator_type >::construct(m_allocator, l_new_node, std::forward<TArgs>(p_args)...);

					l_new_node->m_next = p_next;
					l_new_node->m_prev = p_prev;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &l_new_node->m_next);
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &l_new_node->m_prev);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_next->m_prev);
					p_next->m_prev = l_new_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_next->m_prev);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_prev->m_next);
					p_prev->m_next = l_new_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_prev->m_next);

					p_prev = l_new_node;
					if(!l_first_inserted)
					{
						l_first_inserted = l_new_node;
					}
				}

				if(l_swap_head)
				{
					_swap_head(l_first_inserted);
				}

				base_type::m_size += l_count;

				return l_first_inserted;
			}

			node_type* _new_node(node_type* p_prev, node_type* p_next, this_type& p_other, node_type* p_new_node)
			{
				bcAssert(p_prev == nullptr || p_next == nullptr);

				const bool l_swap_head = p_next == m_head;
				
				// update other head if it is required
				if (p_other.m_head == p_new_node)
				{
					node_type* l_new_node_next = p_other.iterator_increment(p_new_node);

					bc_allocator_traits< internal_allocator_type >::unregister_pointer(p_other.m_allocator, &p_other.m_head);
					p_other.m_head = l_new_node_next;
					if (p_other.m_head)
					{
						bc_allocator_traits< internal_allocator_type >::register_pointer(p_other.m_allocator, &p_other.m_head);
					}
				}

				bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_new_node->m_next);
				bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_new_node->m_prev);

				if (p_other.m_size != 1)
				{
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_new_node->m_prev->m_next);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_new_node->m_next->m_prev);
					p_new_node->m_prev->m_next = p_new_node->m_next;
					p_new_node->m_next->m_prev = p_new_node->m_prev;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_new_node->m_prev->m_next);
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_new_node->m_next->m_prev);
				}

				if (!p_prev && !p_next && !m_head)
				{
					_new_head(p_new_node);
				}
				else
				{
					if (!p_prev && p_next)
					{
						p_prev = p_next->m_prev;
					}

					if (!p_next && p_prev)
					{
						p_next = p_prev->m_next;
					}

					if (!p_prev && !p_next && m_head)
					{
						p_prev = m_head->m_prev;
						p_next = p_prev->m_next;
					}

					p_new_node->m_next = p_next;
					p_new_node->m_prev = p_prev;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_new_node->m_next);
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_new_node->m_prev);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_next->m_prev);
					p_next->m_prev = p_new_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_next->m_prev);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_prev->m_next);
					p_prev->m_next = p_new_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_prev->m_next);
				}

				if(l_swap_head)
				{
					_swap_head(p_new_node);
				}

				base_type::m_size += 1;
				p_other.m_size -= 1;
				
				return p_new_node;
			}

			template< typename TInputIterator >
			node_type* _new_node(node_type* p_prev, node_type* p_next, TInputIterator p_first, TInputIterator p_last, std::input_iterator_tag&)
			{
				using input_iterator_reference = typename std::iterator_traits<TInputIterator>::reference;

				base_type::template check_iterator< TInputIterator >();

				node_type* l_first_inserted = nullptr;
				node_type* l_last_inserted = nullptr;

				std::for_each(p_first, p_last, [=, &l_first_inserted, &l_last_inserted](input_iterator_reference p_value)->void
				{
					l_last_inserted = _new_node(p_prev, p_next, 1, std::forward<input_iterator_reference>(p_value));
					if (!l_first_inserted)
					{
						l_first_inserted = l_last_inserted;
					}
				});

				return l_first_inserted;
			}

			node_type* _free_node(node_type* p_position, size_type p_count)
			{
				bcAssert(p_count <= base_type::m_size);

				node_type* l_next = p_position;
				for (size_type l_c = 0; l_c < p_count; ++l_c)
				{
					if(p_position == nullptr)
					{
						break;
					}

					l_next = iterator_increment(p_position);

					if (m_head == p_position)
					{
						bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &m_head);
						m_head = l_next;
						if (m_head)
						{
							bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_head);
						}
					}

					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_position->m_next);
					bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_position->m_prev);

					if(base_type::m_size != 1)
					{
						bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_position->m_prev->m_next);
						bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_position->m_next->m_prev);
						p_position->m_prev->m_next = p_position->m_next;
						p_position->m_next->m_prev = p_position->m_prev;
						bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_position->m_prev->m_next);
						bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_position->m_next->m_prev);
					}

					bc_allocator_traits< internal_allocator_type >::destroy(m_allocator, p_position);
					bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, p_position);

					p_position = l_next;
					--base_type::m_size;
				}

				return l_next;
			}

			node_pointer m_head;
			internal_allocator_type m_allocator;
		};

		template< typename T, class TAllocator = bc_allocator< T > >
		class bc_list : public bc_list_base< T, TAllocator >
		{
			bc_make_iterators_friend(bc_list)

		public:
			using this_type = bc_list<T, TAllocator>;
			using base_type = bc_list_base< T, TAllocator >;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;
			using node_type = typename base_type::node_type;
			using iterator = bc_bidirectional_iterator< this_type >;
			using const_iterator = bc_const_bidirectional_iterator< this_type >;
			using reverse_iterator = bc_reverse_iterator<iterator>;
			using const_reverse_iterator = bc_reverse_iterator<const_iterator>;

		protected:
			using node_type = typename base_type::node_type;
			using node_pointer = typename base_type::node_pointer;
			using internal_allocator_type = typename base_type::internal_allocator_type;

		public:
			explicit bc_list(const allocator_type& p_allocator = allocator_type());

			explicit bc_list(size_type p_count, const allocator_type& p_allocator = allocator_type());

			bc_list(size_type p_count, const value_type& p_value, const allocator_type& p_allocator = allocator_type());

			template< typename TInputIterator >
			bc_list(TInputIterator p_first, TInputIterator p_last, const allocator_type p_allocator = allocator_type());

			bc_list(std::initializer_list< value_type > p_initializer, const allocator_type& p_allocator = allocator_type());

			bc_list(const this_type& p_other);

			bc_list(const this_type& p_other, const allocator_type& p_allocator);

			bc_list(this_type&& p_other);

			bc_list(this_type&& p_other, const allocator_type& p_allocator);

			~bc_list();

			this_type& operator =(const this_type& p_other);

			this_type& operator =(this_type&& p_other);

			this_type& operator =(std::initializer_list< value_type > p_initializer);

			void assign(size_type p_count, const value_type& p_value);

			template< typename TInputIterator >
			void assign(TInputIterator p_first, TInputIterator p_last);

			void assign(std::initializer_list< value_type > p_initializer);

			allocator_type get_allocator() const;

			reference front();

			const_reference front() const;

			reference back();

			const_reference back() const;

			iterator begin();

			const_iterator begin() const;

			const_iterator cbegin() const;

			iterator end();

			const_iterator end() const;

			const_iterator cend() const;

			reverse_iterator rbegin();

			const_reverse_iterator rbegin() const;

			const_reverse_iterator crbegin() const;

			reverse_iterator rend();

			const_reverse_iterator rend() const;

			const_reverse_iterator crend() const;

			void clear();

			iterator insert(const_iterator p_position, const value_type& p_value);

			iterator insert(const_iterator p_position, value_type&& p_value);

			iterator insert(const_iterator p_position, size_type p_count, value_type&& p_value);

			template< typename TInputIterator >
			iterator insert(const_iterator p_position, TInputIterator p_first, TInputIterator p_last);

			iterator insert(const_iterator p_position, std::initializer_list< value_type > p_initializer);

			template< typename ...TArgs >
			iterator emplace(const_iterator p_position, TArgs&&... p_args);

			iterator erase(const_iterator p_position);

			iterator erase(const_iterator p_first, const_iterator p_last);

			void push_back(const value_type& p_value);

			void push_back(value_type&& p_value);

			template< typename ...TArgs >
			void emplace_back(TArgs&&... p_args);

			void pop_back();

			void push_front(const value_type& p_value);

			void push_front(value_type&& p_value);

			template< class ...Args >
			void emplace_front(Args&&... p_args);

			void pop_front();

			void resize(size_type p_count);

			void resize(size_type p_count, const value_type& p_value);

			void swap(this_type& p_other) noexcept;

			void merge(this_type& p_other);

			void merge(this_type&& p_other);

			template <class TCompare>
			void merge(this_type& p_other, TCompare p_comp);

			template <class TCompare>
			void merge(this_type&& p_other, TCompare p_comp);

			void splice(const_iterator p_pos, this_type& p_other);

			void splice(const_iterator p_pos, this_type&& p_other);
			
			void splice(const_iterator p_pos, this_type& p_other, const_iterator p_iterator);
			
			void splice(const_iterator p_pos, this_type&& p_other, const_iterator p_iterator);
			
			void splice(const_iterator p_pos, this_type& p_other, const_iterator p_first, const_iterator p_last);
			
			void splice(const_iterator p_pos, this_type&& p_other, const_iterator p_first, const_iterator p_last);

			void remove(const value_type& p_value);

			template< class TUnaryPredicate >
			void remove_if(TUnaryPredicate p_predicate);

			void reverse();

			void unique();
			
			template< class TBinaryPredicate >
			void unique(TBinaryPredicate p_predicate);

			void sort();

			template< class TCompare >
			void sort(TCompare p_comp);

		private:
			void _assign(const this_type& p_other, const allocator_type* p_allocator);

			void _assign(this_type&& p_other, const allocator_type* p_allocator);
		};

		template< typename T, template< typename > class TAllocator >
		using bc_list_a = bc_list< T, TAllocator< T > >;

		template< typename T >
		using bc_list_program = bc_list_a< T, bc_allocator_program >;

		template< typename T >
		using bc_list_frame = bc_list_a< T, bc_allocator_frame >;

		template< typename T >
		using bc_list_movale = bc_list_a< T, bc_allocator_movable >;

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(const allocator_type& p_allocator)
			: bc_list_base(p_allocator)
		{

		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(size_type p_count, const allocator_type& p_allocator)
			: bc_list_base(p_allocator)
		{
			base_type::_new_node(nullptr, nullptr, p_count);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(size_type p_count, const value_type& p_value, const allocator_type& p_allocator)
			:bc_list_base(p_allocator)
		{
			base_type::_new_node(nullptr, nullptr, p_count, p_value);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		bc_list<T, TAllocator>::bc_list(TInputIterator p_first, TInputIterator p_last, const allocator_type p_allocator)
			:bc_list_base(p_allocator)
		{
			base_type::_new_node(nullptr, nullptr, p_first, p_last, std::iterator_traits<TInputIterator>::iterator_category());
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(std::initializer_list<value_type> p_initializer, const allocator_type& p_allocator)
			:bc_list_base(p_allocator)
		{
			base_type::_new_node
			(
				nullptr,
				nullptr,
				std::begin(p_initializer), 
				std::end(p_initializer), 
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(const this_type& p_other)
		{
			_assign(p_other, nullptr);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(const this_type& p_other, const allocator_type& p_allocator)
		{
			_assign(p_other, &p_allocator);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(this_type&& p_other)
		{
			_assign(std::move(p_other), nullptr);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::bc_list(this_type&& p_other, const allocator_type& p_allocator)
		{
			_assign(std::move(p_other), &p_allocator);
		}

		template< typename T, class TAllocator >
		bc_list<T, TAllocator>::~bc_list()
		{
			clear();
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::this_type& bc_list<T, TAllocator>::operator=(const this_type& p_other)
		{
			_assign(p_other, nullptr);

			return *this;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::this_type& bc_list<T, TAllocator>::operator=(this_type&& p_other)
		{
			_assign(std::move(p_other), nullptr);

			return *this;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::this_type& bc_list<T, TAllocator>::operator=(std::initializer_list<value_type> p_initializer)
		{
			clear();

			base_type::_new_node
			(
				nullptr,
				nullptr,
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);

			return *this;
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::assign(size_type p_count, const value_type& p_value)
		{
			clear();

			base_type::_new_node(nullptr, nullptr, p_count, p_value);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		void bc_list<T, TAllocator>::assign(TInputIterator p_first, TInputIterator p_last)
		{
			clear();

			base_type::_new_node(nullptr, nullptr, p_first, p_last, typename std::iterator_traits< TInputIterator >::iterator_category());
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::assign(std::initializer_list<value_type> p_initializer)
		{
			clear();

			base_type::_new_node
			(
				nullptr,
				nullptr,
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list<value_type>::iterator >::iterator_category()
			);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::allocator_type bc_list<T, TAllocator>::get_allocator() const
		{
			return typename bc_allocator_traits< internal_allocator_type >::template rebind_alloc< value_type >::other(base_type::m_allocator);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::reference bc_list<T, TAllocator>::front()
		{
			bcAssert(base_type::m_size > 0);

			return base_type::m_head->m_value;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reference bc_list<T, TAllocator>::front() const
		{
			bcAssert(base_type::m_size > 0);

			return base_type::m_head->m_value;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::reference bc_list<T, TAllocator>::back()
		{
			bcAssert(base_type::m_size > 0);

			return base_type::m_head->m_prev->m_value;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reference bc_list<T, TAllocator>::back() const
		{
			bcAssert(base_type::m_size > 0);

			return *base_type::m_head->m_prev->m_value;
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::begin()
		{
			return iterator(this, base_type::m_head);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_iterator bc_list<T, TAllocator>::begin() const
		{
			return const_iterator(this, base_type::m_head);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_iterator bc_list<T, TAllocator>::cbegin() const
		{
			return const_iterator(this, base_type::m_head);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::end()
		{
			return iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_iterator bc_list<T, TAllocator>::end() const
		{
			return const_iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_iterator bc_list<T, TAllocator>::cend() const
		{
			return const_iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::reverse_iterator bc_list<T, TAllocator>::rbegin()
		{
			if(base_type::m_head)
			{
				return reverse_iterator(iterator(this, base_type::m_head->m_prev));
			}
			else
			{
				return reverse_iterator(iterator(this, nullptr));
			}
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reverse_iterator bc_list<T, TAllocator>::rbegin() const
		{
			if (base_type::m_head)
			{
				return const_reverse_iterator(const_iterator(this, base_type::m_head->m_prev));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, nullptr));
			}
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reverse_iterator bc_list<T, TAllocator>::crbegin() const
		{
			if (base_type::m_head)
			{
				return const_reverse_iterator(const_iterator(this, base_type::m_head->m_prev));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, nullptr));
			}
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::reverse_iterator bc_list<T, TAllocator>::rend()
		{
			return reverse_iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reverse_iterator bc_list<T, TAllocator>::rend() const
		{
			return const_reverse_iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::const_reverse_iterator bc_list<T, TAllocator>::crend() const
		{
			return const_reverse_iterator(this, nullptr);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::clear()
		{
			base_type::_free_node(base_type::m_head, base_type::m_size);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::insert(const_iterator p_position, const value_type& p_value)
		{
			node_type* l_node = base_type::_new_node(nullptr, p_position.get_node(), 1, p_value);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::insert(const_iterator p_position, value_type&& p_value)
		{
			node_type* l_node = base_type::_new_node(nullptr, p_position.get_node(), 1, std::move(p_value));

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::insert(const_iterator p_position, size_type p_count, value_type&& p_value)
		{
			node_type* l_node = base_type::_new_node(nullptr, p_position.get_node(), p_count, std::move(p_value));

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::insert(const_iterator p_position, TInputIterator p_first, TInputIterator p_last)
		{
			node_type* l_node = base_type::_new_node(nullptr, p_position.get_node(), p_first, p_last, std::bidirectional_iterator_tag());

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::insert(const_iterator p_position, std::initializer_list<value_type> p_initializer)
		{
			node_type* l_node = base_type::_new_node
			(
				nullptr,
				p_position.get_node(),
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		template< typename ... TArgs >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::emplace(const_iterator p_position, TArgs&&... p_args)
		{
			node_type* l_node = base_type::_new_node(nullptr, p_position.get_node(), 1, std::forward<TArgs>(p_args)...);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::erase(const_iterator p_position)
		{
			node_type* l_node = base_type::_free_node(p_position.get_node(), 1);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_list<T, TAllocator>::iterator bc_list<T, TAllocator>::erase(const_iterator p_first, const_iterator p_last)
		{
			size_type l_count = std::distance(p_first, p_last);

			node_type* l_node = base_type::_free_node(p_first.get_node(), l_count);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::push_back(const value_type& p_value)
		{
			if(base_type::m_head)
			{
				base_type::_new_node(base_type::m_head->m_prev, nullptr, 1, p_value);
			}
			else
			{
				base_type::_new_node(nullptr, nullptr, 1, p_value);
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::push_back(value_type&& p_value)
		{
			if (base_type::m_head)
			{
				base_type::_new_node(base_type::m_head->m_prev, nullptr, 1, std::move(p_value));
			}
			else
			{
				base_type::_new_node(nullptr, nullptr, 1, std::move(p_value));
			}
		}

		template< typename T, class TAllocator >
		template< typename ...TArgs >
		void bc_list<T, TAllocator>::emplace_back(TArgs&&... p_args)
		{
			if (base_type::m_head)
			{
				base_type::_new_node(base_type::m_head->m_prev, nullptr, 1, std::forward<TArgs>(p_args)...);
			}
			else
			{
				base_type::_new_node(nullptr, nullptr, 1, std::forward<TArgs>(p_args)...);
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::pop_back()
		{
			if (base_type::m_head)
			{
				base_type::_free_node(base_type::m_head->m_prev, 1);
			}
			else
			{
				base_type::_free_node(base_type::m_head, 1);
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::push_front(const value_type& p_value)
		{
			base_type::_new_node(nullptr, base_type::m_head, 1, p_value);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::push_front(value_type&& p_value)
		{
			base_type::_new_node(nullptr, base_type::m_head, 1, std::move(p_value));
		}

		template< typename T, class TAllocator >
		template< class ...TArgs >
		void bc_list<T, TAllocator>::emplace_front(TArgs&&... p_args)
		{
			base_type::_new_node(nullptr, base_type::m_head, 1, std::forward<TArgs>(p_args)...);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::pop_front()
		{
			base_type::_free_node(base_type::m_head->m_next, 1);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::resize(size_type p_count)
		{
			size_type l_size = base_type::size();
			size_type l_count = p_count;

			while (l_count < l_size)
			{
				pop_back();
				--l_count;
			}

			while (l_count > l_size)
			{
				emplace_back();
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::resize(size_type p_count, const value_type& p_value)
		{
			size_type l_size = base_type::size();
			size_type l_count = p_count;

			while (l_count < l_size)
			{
				pop_back();
				--l_count;
			}

			while (l_count > l_size)
			{
				push_back(p_value);
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::swap(this_type& p_other) noexcept
		{
			using std::swap;

			std::swap(base_type::m_size, p_other.m_size);
			swap(base_type::m_allocator, p_other.m_allocator);

			if(p_other.m_head)
			{
				bc_allocator_traits< internal_allocator_type >::unregister_pointer(p_other.m_allocator, &p_other.m_head);
			}
			if(base_type::m_head)
			{
				bc_allocator_traits< internal_allocator_type >::unregister_pointer(base_type::m_allocator, &(base_type::m_head));
			}

			std::swap(base_type::m_head, p_other.m_head);

			if (p_other.m_head)
			{
				bc_allocator_traits< internal_allocator_type >::register_pointer(p_other.m_allocator, &p_other.m_head);
			}
			if (base_type::m_head)
			{
				bc_allocator_traits< internal_allocator_type >::register_pointer(base_type::m_allocator, &(base_type::m_head));
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type& p_other)
		{
			this_type::splice(p_pos, p_other, std::begin(p_other), std::end(p_other));
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type&& p_other)
		{
			this_type::splice(p_pos, std::move(p_other), std::begin(p_other), std::end(p_other));
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type& p_other, const_iterator p_iterator)
		{
			this_type::splice(p_pos, std::move(p_other), p_iterator);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type&& p_other, const_iterator p_iterator)
		{
			base_type::_new_node(nullptr, p_pos.get_node(), p_other, p_iterator.get_node());
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type& p_other, const_iterator p_first, const_iterator p_last)
		{
			this_type::splice(p_pos, std::move(p_other), p_first, p_last);
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::splice(const_iterator p_pos, this_type&& p_other, const_iterator p_first, const_iterator p_last)
		{
			auto l_node_to_start_before = p_pos.get_node();
			auto l_inserted_node = static_cast<node_type*>(nullptr);

			for(;p_first != p_last;)
			{
				auto l_next = p_first;
				++l_next;

				auto l_prev_node = l_inserted_node;
				auto l_next_node = l_node_to_start_before;

				l_inserted_node = base_type::_new_node(l_prev_node, l_next_node, p_other, p_first.get_node()); // TODO can be improved by bulk add

				l_node_to_start_before = nullptr;
				
				p_first = l_next;
			}
		}

		template< typename T, class TAllocator >
		void bc_list<T, TAllocator>::remove(const value_type& p_value)
		{
			if(!base_type::m_head)
			{
				return;
			}

			node_type* l_node = base_type::m_head;
			do
			{
				if(l_node->m_value == p_value)
				{
					base_type::_free_node(l_node, 1);
				}

				l_node = l_node->m_next;
			}
			while (l_node && l_node != base_type::m_head);
		}

		template< typename T, class TAllocator >
		template< class TUnaryPredicate >
		void bc_list<T, TAllocator>::remove_if(TUnaryPredicate p_predicate)
		{
			if (!base_type::m_head)
			{
				return;
			}

			node_type* l_node = base_type::m_head;
			do
			{
				if (p_predicate(l_node->m_value))
				{
					l_node = base_type::_free_node(l_node, 1);
				}
			} while (l_node && l_node != base_type::m_head);
		}

		template< typename T, class TAllocator >
		void bc_list< T, TAllocator >::_assign(const this_type& p_other, const allocator_type* p_allocator)
		{
			// Clear content before allocator change
			clear();

			if (p_allocator)
			{
				base_type::m_allocator = *p_allocator;
			}
			else
			{
				base_type::m_allocator = p_other.m_allocator;
			}

			base_type::_new_node(nullptr, base_type::m_head, std::begin(p_other), std::end(p_other), std::bidirectional_iterator_tag());
		}

		template< typename T, class TAllocator >
		void bc_list< T, TAllocator >::_assign(this_type&& p_other, const allocator_type* p_allocator)
		{
			// Clear content before allocator change
			clear();

			if (p_allocator)
			{
				base_type::m_allocator = *p_allocator;
			}
			else
			{
				base_type::m_allocator = std::move(p_other.m_allocator);
			}

			if(p_other.m_head)
			{
				bc_allocator_traits< internal_allocator_type >::unregister_pointer(p_other.m_allocator, &p_other.m_head);
			}

			base_type::m_size = p_other.m_size;
			base_type::m_head = p_other.m_head;

			if(base_type::m_head)
			{
				bc_allocator_traits< internal_allocator_type >::register_pointer(base_type::m_allocator, &(base_type::m_head));
			}

			p_other.m_size = 0;
			p_other.m_head = nullptr;
		}

		template< typename T, typename TAllocator >
		void swap(bc_list< T, TAllocator >& p_first, bc_list< T, TAllocator >& p_second) noexcept
		{
			p_first.swap(p_second);
		}
	}
}