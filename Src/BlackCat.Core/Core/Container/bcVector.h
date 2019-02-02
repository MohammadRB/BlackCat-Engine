// [12/20/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"

namespace black_cat
{
	namespace core
	{
		template < typename T, class TAllocator >
		class bc_vector_base : public bc_container< T, TAllocator >
		{
		public:
			using this_type = bc_vector_base;
			using base_type = bc_container< T, TAllocator >;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;

		protected:
			using node_type = bc_container_node< value_type >;
			using node_pointer = typename std::pointer_traits< pointer >::template rebind< node_type >;
			using internal_allocator_type = typename bc_allocator_traits< allocator_type >::template rebind_alloc< node_type >::other;

		public:
			bc_vector_base(const allocator_type& p_allocator = allocator_type())
				:m_capacity(0),
				 m_first(nullptr),
				 m_allocator(p_allocator)
			{
			}

			~bc_vector_base() = default;

			size_type capacity() const
			{
				return m_capacity;
			};

		protected:
			bool iterator_validate(const node_type* p_node) const noexcept(true)
			{
				return p_node && p_node >= m_first && p_node <= _get_end();
			}

			bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept(true)
			{
				return p_first == p_second ? 0 : p_first > p_second ? 1 : -1;
			}

			value_type* iterator_dereference(node_type* p_node) const noexcept(true)
			{
				return std::addressof(p_node->m_value);
			}

			node_type* iterator_increment(node_type* p_node) const noexcept(true)
			{
				return ++p_node;
			}

			node_type* iterator_decrement(node_type* p_node) const noexcept(true)
			{
				return --p_node;
			}

			node_type* iterator_increment(node_type* p_node, difference_type p_step) const noexcept(true)
			{
				return p_node + p_step;
			}

			node_type* iterator_decrement(node_type* p_node, difference_type p_step) const noexcept(true)
			{
				return p_node - p_step;
			}

			void iterator_swap(node_type** p_first, node_type** p_second) const noexcept(true)
			{
				std::swap(*p_first, *p_second);
			}

			node_type* _get_end() const
			{
				return m_first + base_type::m_size;
			}

			void _move_elements(node_type* p_dest, node_type* p_src, size_type p_count)
			{
				for (bcUINT32 l_i = 0; l_i < p_count; ++l_i)
				{
					bc_allocator_traits< internal_allocator_type >::construct
					(
						m_allocator,
						p_dest + l_i,
						std::move(*(p_src + l_i))
					);

					//// If elements has been copied call their destruction
					//if (!std::is_move_constructable<node_type>::value)
					{
						bc_allocator_traits< internal_allocator_type >::destroy
						(
							m_allocator,
							p_src + l_i
						);
					}
				}
			}

			void _move_if_noexcept_elements(node_type* p_dest, node_type* p_src, size_type p_count, std::true_type)
			{
				for (bcUINT32 l_i = 0; l_i < p_count; ++l_i)
				{
					bc_allocator_traits<internal_allocator_type>::construct
					(
						m_allocator,
						p_dest + l_i,
						std::move_if_noexcept(*(p_src + l_i))
					);

					bc_allocator_traits< internal_allocator_type >::destroy
					(
						m_allocator,
						p_src + l_i
					);
				}
			}

			void _move_if_noexcept_elements(node_type* p_dest, node_type* p_src, size_type p_count, std::false_type)
			{
				for (bcUINT32 l_i = 0; l_i < p_count; ++l_i)
				{
					bc_allocator_traits<internal_allocator_type>::construct
					(
						m_allocator,
						p_dest + l_i,
						std::move(*(p_src + l_i))
					);

					bc_allocator_traits< internal_allocator_type >::destroy
					(
						m_allocator,
						p_src + l_i
					);
				}
			}

			void _change_capacity(size_type p_new_capacity)
			{
				node_type* l_heap = static_cast< node_type* >(bc_allocator_traits< internal_allocator_type >::allocate
				(
					m_allocator,
					p_new_capacity
				));
				
				if (m_first)
				{
					try
					{
						_move_if_noexcept_elements
						(
							l_heap,
							m_first,
							std::min< size_type >(base_type::m_size, p_new_capacity),
							typename std::is_copy_constructible< value_type >::type()
						);
					}
					catch (...)
					{
						bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, l_heap);
						throw;
					}
					
					bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, m_first);
				}

				m_first = l_heap;
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &m_first);
				m_capacity = p_new_capacity;
			}

			void _increase_capacity(size_type p_count_to_add)
			{
				size_type l_reserved_count = m_capacity - base_type::m_size;

				if(p_count_to_add <= l_reserved_count)
				{
					return;
				}

				size_type l_new_capacity = std::pow(2, std::ceil(std::log2(std::max(m_capacity + p_count_to_add, 2U))));
				
				_change_capacity(l_new_capacity);
			};

			void _decrease_capacity()
			{
				if (m_capacity > base_type::m_size)
				{
					if(base_type::m_size != 0)
					{
						_change_capacity(base_type::m_size);
					}
					// If vector is empty, deallocate it's buffer because '_change_capacity' function 
					// always allocate a buffer even if new capacity is zero
					else
					{
						bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, m_first);
						m_first = nullptr;
						m_capacity = 0;
					}
				}
			}

			template< typename ...TArgs >
			node_type* _new_node(node_type* p_position, size_type p_count, TArgs&&... p_args)
			{
				bcAssert(p_position >= m_first && p_position <= _get_end());
				node_type* l_first = m_first;

				_increase_capacity(p_count);

				// _increase_capacity method may change pointers, so if it occur we correct p_position
				p_position = m_first + (p_position - l_first);
				node_type* l_position = p_position;
				size_type l_count = p_count;

				if (l_position != _get_end())
				{
					_move_elements(l_position + p_count, l_position, (_get_end() - l_position));
				}

				for (; l_count > 0; --l_count, ++l_position)
				{
					bc_allocator_traits< internal_allocator_type >::template construct(m_allocator, l_position, std::forward<TArgs>(p_args)...);
				}

				base_type::m_size += p_count;

				return p_position;
			}

			template< typename TInputIterator >
			node_type* _new_node(node_type* p_position, TInputIterator p_first, TInputIterator p_last, std::input_iterator_tag&)
			{
				using input_iterator_reference = typename std::iterator_traits<TInputIterator>::reference;

				bcAssert(p_position >= m_first && p_position <= _get_end());

				base_type::template check_iterator< TInputIterator >();

				size_type l_count = 0;
				node_type* l_last_inserted = p_position;

				std::for_each(p_first, p_last, [=, &l_last_inserted, &l_count](input_iterator_reference p_value)
				{
					l_last_inserted = _new_node(l_last_inserted, 1, std::forward<input_iterator_reference>(p_value));
					++l_last_inserted;
					++l_count;
				});

				base_type::m_size += l_count;

				return l_last_inserted - l_count;
			}

			template< typename TInputIterator >
			node_type* _new_node(node_type* p_position, TInputIterator p_first, TInputIterator p_last, std::forward_iterator_tag&)
			{
				using input_iterator_reference = typename std::iterator_traits<TInputIterator>::reference;

				bcAssert(p_position >= m_first && p_position <= _get_end());

				base_type::template check_iterator< TInputIterator >();

				difference_type l_count = std::distance(p_first, p_last);
				node_type* l_first = m_first;

				_increase_capacity(l_count);

				// _increase_capacity method may change pointers, so if it has occurred we must correct p_position
				p_position = m_first + (p_position - l_first);
				node_type* l_position = p_position;

				if (l_position != _get_end())
				{
					_move_elements(l_position + l_count, l_position, (_get_end() - l_position));
				}

				std::for_each(p_first, p_last, [=, &l_position](input_iterator_reference p_value)
				{
					bc_allocator_traits< internal_allocator_type >::template construct
					(
						m_allocator, l_position++, std::forward<input_iterator_reference>(p_value)
					);
				});

				base_type::m_size += l_count;

				return p_position;
			}

			node_type* _free_node(node_type* p_position, size_type p_count)
			{
				bcAssert(p_position >= m_first && p_position + p_count <= _get_end());

				size_type l_count = p_count;
				while (l_count-- > 0)
				{
					bc_allocator_traits< internal_allocator_type >::destroy(m_allocator, p_position + l_count);
				}

				if (p_position + p_count != _get_end())
				{
					_move_elements(const_cast<node_type*>(p_position), p_position + p_count, (_get_end() - (p_position + p_count)));
				}

				base_type::m_size -= p_count;

				return p_position;
			};

			size_type m_capacity;
			node_pointer m_first;
			internal_allocator_type m_allocator;

		private:
		};

		template < typename T, class TAllocator = bc_allocator< T > >
		class bc_vector : public bc_vector_base< T, TAllocator >
		{
			bc_make_iterators_friend(bc_vector);

		public:
			using this_type = bc_vector;
			using base_type = bc_vector_base< T, TAllocator >;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;
			using iterator = bc_random_access_iterator< this_type >;
			using const_iterator = bc_const_random_access_iterator< this_type >;
			using reverse_iterator = bc_reverse_iterator< iterator >;
			using const_reverse_iterator = bc_reverse_iterator< const_iterator >;

		protected:
			using node_type = typename base_type::node_type;
			using node_pointer = typename base_type::node_pointer;
			using internal_allocator_type = typename base_type::internal_allocator_type;

		public:
			explicit bc_vector(const allocator_type& p_allocator = allocator_type());

			explicit bc_vector(size_type p_count, const allocator_type& p_allocator = allocator_type());

			bc_vector(size_type p_count, const value_type& p_value, const allocator_type& p_allocator = allocator_type());

			template< typename TInputIterator >
			bc_vector(TInputIterator p_first, TInputIterator p_last, const allocator_type p_allocator = allocator_type());

			bc_vector(std::initializer_list< value_type > p_initializer, const allocator_type& p_allocator = allocator_type());

			bc_vector(const this_type& p_other);

			bc_vector(const this_type& p_other, const allocator_type& p_allocator);

			bc_vector(this_type&& p_other) noexcept;

			bc_vector(this_type&& p_other, const allocator_type& p_allocator);

			~bc_vector();

			this_type& operator =(const this_type& p_other);

			this_type& operator =(this_type&& p_other) noexcept;

			this_type& operator =(std::initializer_list< value_type > p_initializer);

			void assign(size_type p_count, const value_type& p_value);

			template< typename TInputIterator >
			void assign(TInputIterator p_first, TInputIterator p_last);

			void assign(std::initializer_list< value_type > p_initializer);

			allocator_type get_allocator() const;

			reference at(size_type p_position);

			const_reference at(size_type p_position) const;

			reference operator [](size_type p_position);

			const_reference operator [](size_type p_position) const;

			reference front();

			const_reference front() const;

			reference back();

			const_reference back() const;

			value_type* data();

			const value_type* data() const;

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

			void reserve(size_type p_new_capacity);

			void shrink_to_fit();

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

			void resize(size_type p_count);

			void resize(size_type p_count, const value_type& p_value);

			void swap(this_type& p_other) noexcept;

		protected:
			
		private:
			void _assign(const this_type& p_other, const allocator_type* p_allocator);

			void _assign(this_type&& p_other, const allocator_type* p_allocator);

			node_type& _get_node(size_type p_position) const
			{
				bcAssert(p_position >= 0 && p_position <= base_type::m_size);

				return *(base_type::m_first + p_position);
			}
		};

		template< typename T, template< typename > class TAllocator >
		using bc_vector_a = bc_vector< T, TAllocator< T > >;
		
		template< typename T >
		using bc_vector_program = bc_vector_a< T, bc_allocator_program >;

		template< typename T >
		using bc_vector_frame = bc_vector_a< T, bc_allocator_frame >;

		template< typename T >
		using bc_vector_movale = bc_vector_a< T, bc_allocator_movable >;

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(const allocator_type& p_allocator)
			: bc_vector_base(p_allocator)
		{
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(size_type p_count, const allocator_type& p_allocator)
			: bc_vector_base(p_allocator)
		{
			base_type::_new_node(base_type::m_first, p_count);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(size_type p_count, const value_type& p_value, const allocator_type& p_allocator)
			: bc_vector_base(p_allocator)
		{
			base_type::_new_node(base_type::m_first, p_count, p_value);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		bc_vector<T, TAllocator>::bc_vector(TInputIterator p_first, TInputIterator p_last, const allocator_type p_allocator)
			: bc_vector_base(p_allocator)
		{
			base_type::_new_node(base_type::m_first, p_first, p_last, typename std::iterator_traits< TInputIterator >::iterator_category());
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(std::initializer_list<value_type> p_initializer, const allocator_type& p_allocator)
			: bc_vector_base(p_allocator)
		{
			base_type::_new_node
			(
				base_type::m_first,
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(const this_type& p_other)
			: bc_vector_base()
		{
			_assign(p_other, nullptr);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(const this_type& p_other, const allocator_type& p_allocator)
			: bc_vector_base()
		{
			_assing(p_other, &p_allocator);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(this_type&& p_other) noexcept
			: bc_vector_base()
		{
			_assign(std::move(p_other), nullptr);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::bc_vector(this_type&& p_other, const allocator_type& p_allocator)
			: bc_vector_base()
		{
			_assign(std::move(p_other), &p_allocator);
		}

		template< typename T, class TAllocator >
		bc_vector<T, TAllocator>::~bc_vector()
		{
			clear();
			bc_allocator_traits< internal_allocator_type >::deallocate(base_type::m_allocator, base_type::m_first);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::this_type& bc_vector<T, TAllocator>::operator =(const this_type& p_other)
		{
			_assign(p_other, nullptr);
			return *this;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::this_type& bc_vector<T, TAllocator>::operator =(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other), nullptr);
			return *this;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::this_type& bc_vector<T, TAllocator>::operator =(std::initializer_list<value_type> p_initializer)
		{
			clear();
			base_type::_new_node
			(
				base_type::m_first,
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);

			return *this;
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::assign(size_type p_count, const value_type& p_value)
		{
			clear();
			base_type::_new_node(base_type::m_first, p_count, p_value);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		void bc_vector<T, TAllocator>::assign(TInputIterator p_first, TInputIterator p_last)
		{
			clear();
			base_type::_new_node(base_type::m_first, p_first, p_last, typename std::iterator_traits< TInputIterator >::iterator_category());
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::assign(std::initializer_list<value_type> p_initializer)
		{
			clear();
			base_type::_new_node
			(
				base_type::m_first,
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category()
			);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::allocator_type bc_vector<T, TAllocator>::get_allocator() const
		{
			return typename bc_allocator_traits< internal_allocator_type >::template rebind_alloc< value_type >::other(base_type::m_allocator);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reference bc_vector<T, TAllocator>::at(size_type p_position)
		{
			if (p_position < 0 || p_position >= base_type::size())
			{
				throw std::out_of_range("Index out of range");
			}

			return _get_node(p_position).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reference bc_vector<T, TAllocator>::at(size_type p_position) const
		{
			if (p_position < 0 || p_position >= base_type::size())
			{
				throw std::out_of_range("Index out of range");
			}

			return _get_node(p_position).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reference bc_vector<T, TAllocator>::operator[](size_type p_position)
		{
			return _get_node(p_position).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reference bc_vector<T, TAllocator>::operator[](size_type p_position) const
		{
			return _get_node(p_position).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reference bc_vector<T, TAllocator>::front()
		{
			return _get_node(0).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reference bc_vector<T, TAllocator>::front() const
		{
			return _get_node(0).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reference bc_vector<T, TAllocator>::back()
		{
			return _get_node(base_type::size() - 1).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reference bc_vector<T, TAllocator>::back() const
		{
			return _get_node(base_type::size() - 1).m_value;
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::value_type* bc_vector<T, TAllocator>::data()
		{
			return static_cast< value_type* >(&_get_node(0).m_value);
		}

		template< typename T, class TAllocator >
		const typename bc_vector<T, TAllocator>::value_type* bc_vector<T, TAllocator>::data() const
		{
			return static_cast< value_type* >(&_get_node(0).m_value);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::begin()
		{
			return iterator(this, base_type::m_first);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_iterator bc_vector<T, TAllocator>::begin() const
		{
			return const_iterator(this, base_type::m_first);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_iterator bc_vector<T, TAllocator>::cbegin() const
		{
			return const_iterator(this, base_type::m_first);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::end()
		{
			return iterator(this, base_type::_get_end());
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_iterator bc_vector<T, TAllocator>::end() const
		{
			return const_iterator(this, base_type::_get_end());
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_iterator bc_vector<T, TAllocator>::cend() const
		{
			return const_iterator(this, base_type::_get_end());
		}
		
		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reverse_iterator bc_vector<T, TAllocator>::rbegin()
		{
			auto l_end = base_type::_get_end();
			if (l_end != nullptr)
			{
				return reverse_iterator(iterator(this, l_end - 1));
			}
			else
			{
				return reverse_iterator(iterator(this, l_end));
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reverse_iterator bc_vector<T, TAllocator>::rbegin() const
		{
			auto l_end = base_type::_get_end();
			if (l_end != nullptr)
			{
				return const_reverse_iterator(const_iterator(this, l_end - 1));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, l_end));
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reverse_iterator bc_vector<T, TAllocator>::crbegin() const
		{
			auto l_end = base_type::_get_end();
			if (l_end != nullptr)
			{
				return const_reverse_iterator(const_iterator(this, l_end - 1));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, l_end));
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::reverse_iterator bc_vector<T, TAllocator>::rend()
		{
			auto l_begin = base_type::m_first;
			if (l_begin != nullptr)
			{
				return reverse_iterator(iterator(this, l_begin - 1));
			}
			else
			{
				return reverse_iterator(iterator(this, l_begin));
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reverse_iterator bc_vector<T, TAllocator>::rend() const
		{
			auto l_begin = base_type::m_first;
			if (l_begin != nullptr)
			{
				return const_reverse_iterator(const_iterator(this, l_begin - 1));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, l_begin));
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::const_reverse_iterator bc_vector<T, TAllocator>::crend() const
		{
			auto l_begin = base_type::m_first;
			if (l_begin != nullptr)
			{
				return const_reverse_iterator(const_iterator(this, l_begin - 1));
			}
			else
			{
				return const_reverse_iterator(const_iterator(this, l_begin));
			}
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::reserve(size_type p_new_capacity)
		{
			if(p_new_capacity > base_type::capacity())
			{
				base_type::_increase_capacity(p_new_capacity - base_type::capacity());
			}
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::shrink_to_fit()
		{
			base_type::_decrease_capacity();
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::clear()
		{
			if (base_type::m_size > 0)
			{
				base_type::_free_node(base_type::m_first, base_type::size());
			}
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::insert(const_iterator p_position, const value_type& p_value)
		{
			node_type* l_node = base_type::_new_node(static_cast<node_type*>(p_position.get_node()), 1, p_value);
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::insert(const_iterator p_position, value_type&& p_value)
		{
			node_type* l_node = base_type::_new_node(static_cast<node_type*>(p_position.get_node()), 1, std::move(p_value));
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::insert(const_iterator p_position, size_type p_count, value_type&& p_value)
		{
			node_type* l_node = base_type::_new_node(static_cast<node_type*>(p_position.get_node()), p_count, std::move(p_value));
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		template< typename TInputIterator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::insert(const_iterator p_position, TInputIterator p_first, TInputIterator p_last)
		{
			node_type* l_node = base_type::_new_node
			(
				static_cast<node_type*>(p_position.get_node()), 
				p_first, 
				p_last,
				typename std::iterator_traits< TInputIterator >::iterator_category()
			);
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::insert(const_iterator p_position, std::initializer_list<value_type> p_initializer)
		{
			node_type* l_node = base_type::_new_node
			(
				static_cast<node_type*>(p_position.get_node()),
				std::begin(p_initializer),
				std::end(p_initializer),
				typename std::iterator_traits< typename std::initializer_list<value_type>::iterator >::iterator_category()
			);
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		template< typename ... TArgs >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::emplace(const_iterator p_position, TArgs&&... p_args)
		{
			node_type* l_node = base_type::_new_node(static_cast<node_type*>(p_position.get_node()), 1, std::forward<TArgs>(p_args)...);
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::erase(const_iterator p_position)
		{
			node_type* l_node = base_type::_free_node(static_cast<node_type*>(p_position.get_node()), 1);
			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		typename bc_vector<T, TAllocator>::iterator bc_vector<T, TAllocator>::erase(const_iterator p_first, const_iterator p_last)
		{
			difference_type l_count = std::distance(p_first, p_last);
			node_type* l_node = base_type::_free_node(static_cast<node_type*>(p_first.get_node()), l_count);

			return iterator(this, l_node);
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::push_back(const value_type& p_value)
		{
			base_type::_new_node(base_type::m_first + base_type::m_size, 1, p_value);
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::push_back(value_type&& p_value)
		{
			base_type::_new_node(base_type::m_first + base_type::m_size, 1, std::move(p_value));
		}

		template< typename T, class TAllocator >
		template< typename ... TArgs >
		void bc_vector<T, TAllocator>::emplace_back(TArgs&&... p_args)
		{
			base_type::_new_node(base_type::m_first + base_type::m_size, 1, std::forward<TArgs>(p_args)...);
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::pop_back()
		{
			base_type::_free_node(base_type::m_first + base_type::m_size - 1, 1);
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::resize(size_type p_count)
		{
			if (p_count < base_type::size())
			{
				base_type::_free_node(base_type::m_first + p_count, base_type::size() - p_count);
			}
			else if (p_count > base_type::size())
			{
				base_type::_new_node(base_type::m_first + base_type::size(), p_count - base_type::size());
			}
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::resize(size_type p_count, const value_type& p_value)
		{
			if (p_count < base_type::size())
			{
				base_type::_free_node(base_type::m_first + p_count, base_type::size() - p_count);
			}
			else if (p_count > base_type::size())
			{
				base_type::_new_node(base_type::m_first + base_type::size(), p_count - base_type::size(), p_value);
			}
		}

		template< typename T, class TAllocator >
		void bc_vector<T, TAllocator>::swap(this_type& p_other) noexcept
		{
			using std::swap;

			std::swap(base_type::m_size, p_other.m_size);
			std::swap(base_type::m_capacity, p_other.m_capacity);
			swap(base_type::m_allocator, p_other.m_allocator);
			swap(base_type::m_first, p_other.m_first);
		}

		template< typename T, class TAllocator >
		void bc_vector< T, TAllocator >::_assign(const this_type& p_other, const allocator_type* p_allocator)
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

			base_type::_new_node(base_type::m_first, std::begin(p_other), std::end(p_other), std::random_access_iterator_tag());
		}

		template< typename T, class TAllocator >
		void bc_vector< T, TAllocator >::_assign(this_type&& p_other, const allocator_type* p_allocator)
		{
			// Clear content before allocator change
			clear();
			shrink_to_fit();

			if (p_allocator)
			{
				base_type::m_allocator = *p_allocator;
			}
			else
			{
				base_type::m_allocator = std::move(p_other.m_allocator);
			}

			if(p_other.m_first)
			{
				bc_allocator_traits< internal_allocator_type >::unregister_pointer(p_other.m_allocator, &p_other.m_first);
			}

			base_type::m_size = p_other.m_size;
			base_type::m_capacity = p_other.m_capacity;
			base_type::m_first = std::move(p_other.m_first);

			if(base_type::m_first)
			{
				bc_allocator_traits< internal_allocator_type >::register_pointer(base_type::m_allocator, &(base_type::m_first));
			}

			p_other.m_size = 0;
			p_other.m_capacity = 0;
			p_other.m_first = nullptr;
		}

		template< typename T, typename TAllocator >
		void swap(bc_vector< T, TAllocator >& p_first, bc_vector< T, TAllocator >& p_second) noexcept
		{
			p_first.swap(p_second);
		}
	}
}