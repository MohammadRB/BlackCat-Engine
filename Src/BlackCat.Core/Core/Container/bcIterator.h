//  [12/17/2013 MRB]

#pragma once

#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
		template< class TProvider >
		struct bc_input_iterator_provider_traits
		{
		public:
			using provider_type = TProvider;
			using value_type = typename provider_type::value_type;
			using node_type = typename provider_type::node_type;

			static bool validate(const provider_type& p_provider, const node_type* p_node) noexcept
			{
				return p_provider.iterator_validate(p_node);
			}

			static bcINT32 compare(const provider_type& p_provider, const node_type* p_first, const node_type* p_second) noexcept
			{
				return p_provider.iterator_compare(p_first, p_second);
			}

			static value_type* dereference(const provider_type& p_provider, node_type* p_node)
			{
				return p_provider.iterator_dereference(p_node);
			}

			static node_type* increment(const provider_type& p_provider, node_type* p_node)
			{
				return p_provider.iterator_increment(p_node);
			}

			static void swap(const provider_type& p_provider, node_type** p_first, node_type** p_second) noexcept
			{
				p_provider.iterator_swap(p_first, p_second);
			}
		};

		template< typename TProvider >
		struct bc_forward_iterator_provider_traits : public bc_input_iterator_provider_traits< TProvider >
		{
		};

		template< typename TProvider >
		struct bc_bidirectional_iterator_provider_traits : public bc_forward_iterator_provider_traits< TProvider >
		{
		public:
			using provider_type = TProvider;
			using value_type = typename provider_type::value_type;
			using node_type = typename provider_type::node_type;

			static node_type* decrement(const provider_type& p_provider, node_type* p_node)
			{
				return p_provider.iterator_decrement(p_node);
			}
		};

		template< typename TProvider >
		struct bc_random_access_iterator_provider_traits : public bc_bidirectional_iterator_provider_traits< TProvider >
		{
			using provider_type = TProvider;
			using value_type = typename provider_type::value_type;
			using node_type = typename provider_type::node_type;
			using difference_type = typename provider_type::difference_type;
			using bc_bidirectional_iterator_provider_traits< TProvider >::increment;
			using bc_bidirectional_iterator_provider_traits< TProvider >::decrement;

			static node_type* increment(const provider_type& p_provider, node_type* p_node, difference_type p_step)
			{
				return p_provider.iterator_increment(p_node, p_step);
			}

			static node_type* decrement(const provider_type& p_provider, node_type* p_node, difference_type p_step)
			{
				return p_provider.iterator_decrement(p_node, p_step);
			}
		};

		template< typename TProvider >
		class bc_input_iterator : public std::iterator
		< 
			std::input_iterator_tag,
			typename TProvider::value_type,
			typename TProvider::difference_type,
			typename TProvider::pointer,
			typename TProvider::reference 
		>
		{
		public:
			using this_type = bc_input_iterator;
			using provider_type = TProvider;
			using value_type = typename bc_input_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_input_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_input_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				:m_provider(p_provider),
				 m_node(p_node)
			{
			}

			bc_input_iterator(const this_type& p_other) noexcept
				:m_provider(p_other.m_provider),
				 m_node(p_other.m_node)
			{
			}

			~bc_input_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_provider = p_other.m_provider;
				m_node = p_other.m_node;

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return bc_input_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) == 0;
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return bc_input_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) != 0;
			}

			reference operator *() const
			{
				bcAssert(bc_input_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *bc_input_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			pointer operator ->() const noexcept
			{
				bcAssert(bc_input_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return bc_input_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			this_type& operator ++()
			{
				m_node = bc_input_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_input_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator ++(bcINT32)
			{
				this_type l_old = *this;

				m_node = bc_input_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_input_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return l_old;
			}

			void swap(this_type& p_other) noexcept
			{
				bc_input_iterator_provider_traits< provider_type >::swap(*m_provider, &m_node, &p_other.m_node);
			}

			node_type* get_node() const noexcept
			{
				return m_node;
			}

		private:
			const provider_type* m_provider;
			node_type* m_node;
		};

		template< typename TProvider >
		class bc_const_forward_iterator : public std::iterator<std::forward_iterator_tag,
			typename TProvider::value_type,
			typename TProvider::difference_type,
			typename TProvider::pointer,
			typename TProvider::reference >
		{
		public:
			using this_type = bc_const_forward_iterator;
			using provider_type = TProvider;
			using value_type = typename bc_forward_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_forward_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_const_forward_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				:m_provider(p_provider),
				m_node(p_node)
			{
			}

			bc_const_forward_iterator(const this_type& p_other) noexcept
				:m_provider(p_other.m_provider),
				m_node(p_other.m_node)
			{
			}

			~bc_const_forward_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_provider = p_other.m_provider;
				m_node = p_other.m_node;

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return bc_forward_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) == 0;
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return bc_forward_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) != 0;
			}

			reference operator *() const
			{
				bcAssert(bc_forward_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *bc_forward_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			pointer operator ->() const noexcept
			{
				bcAssert(bc_forward_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return bc_forward_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			this_type& operator ++()
			{
				m_node = bc_forward_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_forward_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator ++(bcINT32)
			{
				this_type l_old = *this;

				m_node = bc_forward_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_forward_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return l_old;
			}

			void swap(this_type& p_other) noexcept
			{
				bc_forward_iterator_provider_traits< provider_type >::swap(*m_provider, &m_node, &p_other.m_node);
			}

			node_type* get_node() const noexcept
			{
				return m_node;
			}

		private:
			const provider_type* m_provider;
			node_type* m_node;
		};

		template< typename TProvider >
		class bc_forward_iterator : public bc_const_forward_iterator< TProvider >
		{
		public:
			using this_type = bc_forward_iterator;
			using base_type = bc_const_forward_iterator< TProvider >;
			using provider_type = TProvider;
			using value_type = typename bc_forward_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_forward_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_forward_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				: bc_const_forward_iterator(p_provider, p_node)
			{
			}

			bc_forward_iterator(const this_type& p_other) noexcept
				: bc_const_forward_iterator(p_other)
			{
			}

			~bc_forward_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				base_type::operator =(p_other);

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return base_type::operator ==(p_other);
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return base_type::operator !=(p_other);
			}

			reference operator *() const
			{
				return const_cast<reference>(base_type::operator*());
			}

			pointer operator ->() const noexcept
			{
				return const_cast<pointer>(base_type::operator->());
			}

			this_type& operator ++()
			{
				base_type::operator++();

				return *this;
			}

			this_type operator ++(bcINT32)
			{
				this_type l_old = *this;

				base_type::operator++();

				return l_old;
			}

			void swap(this_type& p_other) noexcept
			{
				base_type::swap(p_other);
			}
		};

		template< typename TProvider >
		class bc_const_bidirectional_iterator : public std::iterator< std::bidirectional_iterator_tag,
		    typename TProvider::value_type,
		    typename TProvider::difference_type,
		    typename TProvider::pointer,
		    typename TProvider::reference >
		{
		public:
			using this_type = bc_const_bidirectional_iterator;
			using provider_type = TProvider;
			using value_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_const_bidirectional_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				:m_provider(p_provider),
				m_node(p_node)
			{
			}

			bc_const_bidirectional_iterator(const this_type& p_other) noexcept
				:m_provider(p_other.m_provider),
				m_node(p_other.m_node)
			{
			}

			~bc_const_bidirectional_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_provider = p_other.m_provider;
				m_node = p_other.m_node;

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return bc_bidirectional_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) == 0;
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return bc_bidirectional_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) != 0;
			}

			reference operator *() const
			{
				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *bc_bidirectional_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			pointer operator ->() const noexcept
			{
				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return bc_bidirectional_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			this_type& operator ++()
			{
				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				m_node = bc_bidirectional_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				return *this;
			}

			this_type operator ++(bcINT32)
			{
				this_type l_old = *this;

				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				m_node = bc_bidirectional_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				return l_old;
			}

			this_type& operator --()
			{
				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				m_node = bc_bidirectional_iterator_provider_traits< provider_type >::decrement(*m_provider, m_node);

				return *this;
			}

			this_type operator --(bcINT32)
			{
				this_type l_old = *this;

				bcAssert(bc_bidirectional_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				m_node = bc_bidirectional_iterator_provider_traits< provider_type >::decrement(*m_provider, m_node);

				return l_old;
			}

			void swap(this_type& p_other) noexcept
			{
				bc_bidirectional_iterator_provider_traits< provider_type >::swap(*m_provider, &m_node, &p_other.m_node);
			}

			node_type* get_node() const noexcept
			{
				return m_node;
			}

		private:
			const provider_type* m_provider;
			node_type* m_node;
		};

		template< typename TProvider >
		class bc_bidirectional_iterator : public bc_const_bidirectional_iterator< TProvider >
		{
		public:
			using this_type = bc_bidirectional_iterator;
			using base_type = bc_const_bidirectional_iterator< TProvider >;
			using provider_type = TProvider;
			using value_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_bidirectional_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				: bc_const_bidirectional_iterator(p_provider, p_node)
			{
			}

			bc_bidirectional_iterator(const this_type& p_other) noexcept
				: bc_const_bidirectional_iterator(p_other)
			{
			}

			~bc_bidirectional_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				base_type::operator =(p_other);

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return base_type::operator ==(p_other);
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return base_type::operator !=(p_other);
			}

			reference operator *() const
			{
				return const_cast<reference>(base_type::operator*());
			}

			pointer operator ->() const noexcept
			{
				return const_cast<pointer>(base_type::operator->());
			}

			this_type& operator ++()
			{
				base_type::operator++();

				return *this;
			}

			this_type operator ++(bcINT32)
			{
				this_type l_old = *this;

				base_type::operator++();

				return l_old;
			}

			this_type& operator --()
			{
				base_type::operator--();

				return *this;
			}

			this_type operator --(bcINT32)
			{
				this_type l_old = *this;

				base_type::operator--();

				return l_old;
			}

			void swap(this_type& p_other) noexcept
			{
				base_type::swap(p_other);
			}
		};

		template< typename TProvider >
		class bc_const_random_access_iterator : public std::iterator< std::random_access_iterator_tag,
			typename TProvider::value_type,
			typename TProvider::difference_type,
			typename TProvider::pointer,
			typename TProvider::reference >
		{
		public:
			using this_type = bc_const_random_access_iterator;
			using provider_type = TProvider;
			using value_type = typename bc_random_access_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_random_access_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_const_random_access_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				:m_provider(p_provider),
				m_node(p_node)
			{
			}

			bc_const_random_access_iterator(const this_type& p_other) noexcept
				:m_provider(p_other.m_provider),
				m_node(p_other.m_node)
			{
			}

			~bc_const_random_access_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_provider = p_other.m_provider;
				m_node = p_other.m_node;

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) == 0;
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) != 0;
			}

			reference operator *() const
			{
				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *bc_random_access_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			pointer operator ->() const noexcept
			{
				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return bc_random_access_iterator_provider_traits< provider_type >::dereference(*m_provider, m_node);
			}

			this_type& operator ++()
			{
				m_node = bc_random_access_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator ++(difference_type)
			{
				this_type l_old = *this;

				m_node = bc_random_access_iterator_provider_traits< provider_type >::increment(*m_provider, m_node);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return l_old;
			}

			this_type& operator +=(difference_type p_count)
			{
				m_node = bc_random_access_iterator_provider_traits< provider_type >::increment(*m_provider, m_node, p_count);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator +(difference_type p_count) const
			{
				this_type l_temp = *this;
				l_temp += p_count;

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, l_temp.m_node));

				return l_temp;
			}

			this_type& operator --()
			{
				m_node = bc_random_access_iterator_provider_traits< provider_type >::decrement(*m_provider, m_node);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator --(difference_type)
			{
				this_type l_old = *this;

				m_node = bc_random_access_iterator_provider_traits< provider_type >::decrement(*m_provider, m_node);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return l_old;
			}

			this_type& operator -=(difference_type p_count)
			{
				m_node = bc_random_access_iterator_provider_traits< provider_type >::decrement(*m_provider, m_node, p_count);

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, m_node));

				return *this;
			}

			this_type operator -(difference_type p_count) const
			{
				this_type l_temp = *this;
				l_temp -= p_count;

				bcAssert(bc_random_access_iterator_provider_traits< provider_type >::validate(*m_provider, l_temp.m_node));

				return l_temp;
			}

			difference_type operator -(const this_type& p_other) const
			{
				return m_node - p_other.m_node;
			}

			bool operator <(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_type >::compare(*m_provider, m_node, p_other.m_node) < 0;
			}

			bool operator <=(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_info >::compare(*m_provider, m_node, p_other.m_node) <= 0;
			}

			bool operator >(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_info >::compare(*m_provider, m_node, p_other.m_node) > 0;
			}

			bool operator >=(const this_type& p_other) const noexcept
			{
				return bc_random_access_iterator_provider_traits< provider_info >::compare(*m_provider, m_node, p_other.m_node) >= 0;
			}

			void swap(this_type& p_other) noexcept
			{
				bc_random_access_iterator_provider_traits< provider_type >::swap(*m_provider, &m_node, &p_other.m_node);
			}

			node_type* get_node() const noexcept
			{
				return m_node;
			}
			
		private:
			const provider_type* m_provider;
			node_type* m_node;
		};

		template< typename TProvider >
		class bc_random_access_iterator : public bc_const_random_access_iterator< TProvider >
		{
		public:
			using this_type = bc_random_access_iterator;
			using base_type = bc_const_random_access_iterator< TProvider >;
			using provider_type = TProvider;
			using value_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::value_type;
			using node_type = typename bc_bidirectional_iterator_provider_traits< provider_type >::node_type;

		public:
			bc_random_access_iterator(const provider_type* p_provider, node_type* p_node) noexcept
				: bc_const_random_access_iterator(p_provider, p_node)
			{
			}

			bc_random_access_iterator(const this_type& p_other) noexcept
				: bc_const_random_access_iterator(p_other)
			{
			}

			~bc_random_access_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				base_type::operator =(p_other);

				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return base_type::operator ==(p_other);
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return base_type::operator !=(p_other);
			}

			reference operator *() const
			{
				return const_cast<reference>(base_type::operator*());
			}

			pointer operator ->() const noexcept
			{
				return const_cast<pointer>(base_type::operator->());
			}

			this_type& operator ++()
			{
				base_type::operator++();

				return *this;
			}

			this_type operator ++(difference_type)
			{
				this_type l_old = *this;

				base_type::operator++();

				return l_old;
			}

			this_type& operator +=(difference_type p_count)
			{
				base_type::operator+=(p_count);

				return *this;
			}

			this_type operator +(difference_type p_count) const
			{
				this_type l_temp = *this;
				l_temp += p_count;

				return l_temp;
			}

			this_type& operator --()
			{
				base_type::operator--();

				return *this;
			}

			this_type operator --(difference_type)
			{
				this_type l_old = *this;

				base_type::operator--();

				return l_old;
			}

			this_type& operator -=(difference_type p_count)
			{
				base_type::operator-=(p_count);

				return *this;
			}

			this_type operator -(difference_type p_count) const
			{
				this_type l_temp = *this;
				l_temp -= p_count;

				return l_temp;
			}

			difference_type operator -(const this_type& p_other) const
			{
				return base_type::operator-(p_other);
			}

			bool operator <(const this_type& p_other) const noexcept
			{
				return base_type::operator<(p_other);
			}

			bool operator <=(const this_type& p_other) const noexcept
			{
				return base_type::operator<=(p_other);
			}

			bool operator >(const this_type& p_other) const noexcept
			{
				return base_type::operator>(p_other);
			}

			bool operator >=(const this_type& p_other) const noexcept
			{
				return base_type::operator>=(p_other);
			}

			void swap(this_type& p_other) noexcept
			{
				base_type::swap(p_other);
			}
		};

		template< class TProvider >
		bc_const_random_access_iterator<TProvider> operator +(typename bc_const_random_access_iterator<TProvider>::difference_type p_first,
			bc_const_random_access_iterator<TProvider>& p_second)
		{
			bc_const_random_access_iterator<TProvider> l_temp = p_second;
			l_temp += p_first;

			return l_temp;
		}

		template< class TProvider >
		bc_random_access_iterator<TProvider> operator +(typename bc_random_access_iterator<TProvider>::difference_type p_first,
			bc_random_access_iterator<TProvider>& p_second)
		{
			bc_random_access_iterator<TProvider> l_temp = p_second;
			l_temp += p_first;

			return l_temp;
		}

		/**
		 * \brief Unlike std::reverse_iterator this iterator does not point to one-past-the-end iterator
		 * \tparam TIterator 
		 */
		template< typename TIterator >
		class bc_reverse_iterator : public std::iterator< typename std::iterator_traits<TIterator>::iterator_category,
			typename std::iterator_traits<TIterator>::value_type,
			typename std::iterator_traits<TIterator>::difference_type,
			typename std::iterator_traits<TIterator>::pointer,
			typename std::iterator_traits<TIterator>::reference >
		{
		public:
			using this_type = bc_reverse_iterator;
			using iterator_type = TIterator;
			using pointer = typename std::iterator_traits<TIterator>::pointer;
			using reference = typename std::iterator_traits<TIterator>::reference;

		public:
			explicit bc_reverse_iterator(TIterator p_iterator) noexcept
				: m_iterator(p_iterator)
			{
			}

			bc_reverse_iterator(const this_type& p_other) noexcept
				: m_iterator(p_other.m_iterator)
			{
			}

			~bc_reverse_iterator()
			{
			}

			this_type& operator =(const this_type& p_other) noexcept
			{
				m_iterator = p_other.m_iterator;
				return *this;
			}

			bool operator ==(const this_type& p_other) const noexcept
			{
				return m_iterator == p_other.m_iterator;
			}

			bool operator !=(const this_type& p_other) const noexcept
			{
				return m_iterator != p_other.m_iterator;
			}

			reference operator *() const
			{
				return m_iterator.operator*();
			}

			pointer operator ->() const noexcept
			{
				return m_iterator.operator->();
			}

			this_type& operator ++()
			{
				--m_iterator;
				return *this;
			}

			this_type operator ++(difference_type)
			{
				this_type l_old = *this;

				--m_iterator;

				return l_old;
			}

			this_type& operator +=(difference_type p_count)
			{
				m_iterator -= p_count;
				return *this;
			}

			this_type operator +(difference_type p_count)
			{
				this_type l_old = *this;
				l_old.m_iterator -= p_count;

				return l_old;
			}

			this_type& operator --()
			{
				++m_iterator;
				return *this;
			}

			this_type operator --(difference_type)
			{
				this_type l_old = *this;

				++m_iterator;

				return l_old;
			}

			this_type& operator -=(difference_type p_count)
			{
				m_iterator += p_count;

				return m_iterator;
			}

			this_type operator -(difference_type p_count)
			{
				this_type l_old = *this;

				l_old.m_iterator += p_count;

				return l_old;
			}

			difference_type operator -(this_type& p_other)
			{
				return m_iterator - p_other.m_iterator;
			}

			bool operator <(const this_type& p_other) noexcept
			{
				return m_iterator < p_other.m_iterator;
			}

			bool operator <=(const this_type& p_other) noexcept
			{
				return m_iterator <= p_other.m_iterator;
			}

			bool operator >(const this_type& p_other) noexcept
			{
				return m_iterator > p_other.m_iterator;
			}

			bool operator >=(const this_type& p_other) noexcept
			{
				return m_iterator >= p_other.m_iterator;
			}

			void swap(this_type& p_other) noexcept
			{
				m_iterator.swap(p_other.m_iterator);
			}

			TIterator base() const noexcept
			{
				return m_iterator;
			}

		private:
			TIterator m_iterator;
		};

#define bc_make_iterators_friend(TProvider) \
		friend struct black_cat::core::bc_input_iterator_provider_traits<TProvider>; \
		friend struct black_cat::core::bc_forward_iterator_provider_traits<TProvider>; \
		friend struct black_cat::core::bc_bidirectional_iterator_provider_traits<TProvider>; \
		friend struct black_cat::core::bc_random_access_iterator_provider_traits<TProvider>;
	}
}