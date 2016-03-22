// [1/1/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Container/bcContainer.h"
#include "Core/bcCoreUtility.h"

namespace black_cat
{
	namespace core
	{
		template < class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		class bc_tree_base : public bc_container< std::pair<const TK ,T>, TAllocator >
		{
		public:
			using this_type = bc_tree_base;
			using base_type = bc_container< std::pair<const TK, T>, TAllocator >;
			using key_type = TK;
			using mapped_type = T;
			using key_compare_type = TCompare;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;

		protected:
			struct node;

			using node_type = node;
			using node_pointer = typename std::pointer_traits< pointer >::template rebind< node_type >;
			using internal_allocator_type = typename bc_allocator_traits< allocator_type >::template rebind_alloc< node_type >::other;

		public:
			class value_compare
			{
			public:
				using result_type = bool;
				using first_argument_type = value_type;
				using second_argument_type = value_type;

				result_type operator ()(const first_argument_type& p_first, const second_argument_type p_second)
				{
					return m_compare(p_first.first, p_second.first);
				}

			protected:
				value_compare(key_compare_type p_compare) : m_compare(p_compare)
				{
				}

				key_compare_type m_compare;
			};

			struct node : public bc_container_node< value_type >
			{
				node(const value_type& p_value) noexcept(std::is_nothrow_copy_constructible<bc_container_node<value_type>>::value) 
					: bc_container_node(p_value),
					m_parent(nullptr),
					m_left(nullptr),
					m_right(nullptr),
					m_left_height(0),
					m_right_height(0)
				{
				}

				node(value_type&& p_value) noexcept(std::is_nothrow_move_constructible<bc_container_node<value_type>>::value) 
					: bc_container_node(std::move(p_value)),
					m_parent(nullptr),
					m_left(nullptr),
					m_right(nullptr),
					m_left_height(0),
					m_right_height(0)
				{
				}

				template< typename ...TArgs >
				node(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<bc_container_node<value_type>, TArgs...>::value)
					: bc_container_node(std::forward<TArgs>(p_args)...),
					m_parent(nullptr),
					m_left(nullptr),
					m_right(nullptr),
					m_left_height(0),
					m_right_height(0)
				{
				}

				node(const node& p_other)
					: bc_container_node(p_other.m_value),
					m_parent(p_other.m_parent),
					m_left(p_other.m_left),
					m_right(p_other.m_right),
					m_left_height(p_other.m_left_height),
					m_right_height(p_other.m_right_height)
				{
				}

				node(node&& p_other)
					: bc_container_node(std::move(p_other.m_value)),
					m_parent(p_other.m_parent),
					m_left(p_other.m_left),
					m_right(p_other.m_right),
					m_left_height(p_other.m_left_height),
					m_right_height(p_other.m_right_height)
				{
					p_other.m_parent = nullptr;
					p_other.m_left = nullptr;
					p_other.m_right = nullptr;
				}

				~node() = default;

				node_pointer m_parent;
				node_pointer m_left;
				node_pointer m_right;
				bcUINT16 m_left_height;
				bcUINT16 m_right_height;
			};

			bc_tree_base(const key_compare_type p_compare = key_compare_type(), const allocator_type& p_allocator = allocator_type())
				: m_root(nullptr),
				m_compare(p_compare),
				m_allocator(p_allocator)
			{
			}

			~bc_tree_base() = default;

		protected:
			bool iterator_validate(const node_type* p_node) const noexcept(true)
			{
				return p_node;
			}

			bcINT32 iterator_compare(const node_type* p_first, const node_type* p_second) const noexcept(true)
			{
				return p_first == p_second ? 0 : p_first > p_second ? 1 : -1;
			}

			value_type* iterator_dereference(node_type* p_node) const noexcept(true)
			{
				return std::addressof(p_node->m_value);
			}

			node_type* iterator_increment(node_type* p_node)  const noexcept(true)
			{
				return _node_successor(p_node);
			}

			node_type* iterator_decrement(node_type* p_node)  const noexcept(true)
			{
				return _node_pre_decessor(p_node);
			}

			void iterator_swap(node_type** p_first, node_type** p_second) const noexcept(true)
			{
				std::swap(*p_first, *p_second);
			}

			// Return true if we rotate p_node
			bool _check_balance(node_type*& p_root, node_type* p_node)
			{
				// TODO check for double rotation case
				if (std::abs(p_node->m_left_height - p_node->m_right_height) <= MaxDiff)
					return false;

				node_type* l_child;
				if (p_node->m_left_height > p_node->m_right_height)
				{
					l_child = p_node->m_left;

					if (l_child->m_right)
						p_node->m_left_height = std::max<bcUINT16>(l_child->m_right->m_left_height, l_child->m_right->m_right_height) + 1;
					else
						p_node->m_left_height = 0;

					l_child->m_right_height = std::max<bcUINT16>(p_node->m_left_height, p_node->m_right_height) + 1;

					_node_rotate_right(p_root, p_node);
				}
				else
				{
					l_child = p_node->m_right;

					if (l_child->m_left)
						p_node->m_right_height = std::max<bcUINT16>(l_child->m_left->m_left_height, l_child->m_left->m_right_height) + 1;
					else
						p_node->m_right_height = 0;

					l_child->m_left_height = std::max<bcUINT16>(p_node->m_left_height, p_node->m_right_height) + 1;

					_node_rotate_left(p_root, p_node);
				}

				return true;
			}

			// pNode : added node or node that will be removed, pAddRem : 1 if add a node, -1 if remove a node
			void _refresh_node_heights(node_type*& p_root, node_type*& p_node, bcINT16 p_add_rem)
			{
				// Update parents left and right heights
				node_type* l_curr_node = p_node;
				node_type* l_p_node = l_curr_node->m_parent;

				// If current node is not root
				if (!l_p_node)
					return;

				// Plus or minus one from parent
				if (l_p_node->m_left == l_curr_node)
				{
					l_p_node->m_left_height += p_add_rem;
					// If we want to remove a node, check parent balance
					if (p_add_rem == -1)
					{
						if (_check_balance(p_root, l_p_node))
							// Because we rotate parent, parent has moved one level down, so we move it to up
							l_p_node = l_p_node->m_parent;
					}
				}
				else
				{
					l_p_node->m_right_height += p_add_rem;
					// If we want to remove a node, check parent balance
					if (p_add_rem == -1)
					{
						if (_check_balance(p_root, l_p_node))
							// Because we rotate parent, parent has moved one level down, so we move it to up
							l_p_node = l_p_node->m_parent;
					}
				}

				l_curr_node = l_p_node;
				l_p_node = l_curr_node->m_parent;
				while (l_p_node)
				{
					bcUINT16 l_max = std::max<bcUINT16>(l_curr_node->m_left_height, l_curr_node->m_right_height);
					if (l_p_node->m_left == l_curr_node)
					{
						//if(lMax + 1 == NodeLeftHeight(lPNode))
						//	// No Update require, because right sub-tree is higher
						//	// or equal to left and add node or delete node in left 
						//	// dont impact on parent height
						//	break;
						//else
						l_p_node->m_left_height = l_max + 1;
					}
					else
					{
						//if(lMax + 1 == NodeRightHeight(lPNode))
						//	// No Update require, because left sub-tree is higher
						//	// or equal to right and add node or delete node in right
						//	// dont impact on parent height
						//	break;
						//else
						l_p_node->m_right_height = l_max + 1;
					}

					// If currParent is not balance rotate it to left or right and currect some node heights
					if (_check_balance(p_root, l_p_node))
						// Because we rotated parent, parent have moved to under curr, so we move parent to point to curr. 
						//in next lines parent will become parent of lCurrNode
						l_p_node = l_curr_node;

					l_curr_node = l_p_node;
					l_p_node = l_curr_node->m_parent;
				}
			}

			template < typename TValueType >
			bool _new_node(node_type*& p_root, node_type** p_result, TValueType&& p_value)
			{
				node_type* l_curr_node = p_root;
				node_type* l_p_node = nullptr;

				while (l_curr_node)
				{
					l_p_node = l_curr_node;
					if (m_compare(p_value.first, l_curr_node->m_value.first))
						l_curr_node = l_curr_node->m_left;
					else if (m_compare(l_curr_node->m_value.first, p_value.first))
						l_curr_node = l_curr_node->m_right;
					else
						break;
				}

				// One node with same key has been found
				if (l_curr_node)
				{
					*p_result = l_curr_node;
					return false;
				}

				node_type* l_node = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
				bc_allocator_traits< internal_allocator_type >::construct(
					m_allocator,
					l_node,
					std::forward<TValueType>(p_value));

				l_node->m_left = nullptr;
				l_node->m_right = nullptr;
				l_node->m_left_height = 0;
				l_node->m_right_height = 0;

				l_node->m_parent = l_p_node;
				bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &l_node->m_parent);

				if (!l_p_node)
				{
					p_root = l_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_root);
				}
				else if (m_compare(l_node->m_value.first, l_p_node->m_value.first))
				{
					l_p_node->m_left = l_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &l_p_node->m_left);
				}
				else
				{
					l_p_node->m_right = l_node;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &l_p_node->m_right);
				}

				_refresh_node_heights(p_root, l_node, 1);

				*p_result = l_node;
				return true;
			};

			template < typename ...TArgs >
			bool _new_node(node_type*& p_root, node_type** p_result, TArgs&&... p_args)
			{
				return _new_node(p_root, p_result, value_type(std::forward<TArgs>(p_args)...));
			}
			
			template< typename TInputIterator >
			node_type* _new_node(node_type*& p_root, TInputIterator p_first, TInputIterator p_last, std::input_iterator_tag&)
			{
				base_type::template _check_iterator< TInputIterator >();

				size_type l_count = 0;
				node_type* l_last_inserted = nullptr;

				std::for_each(p_first, p_last, [=, &l_count](typename std::iterator_traits<TInputIterator>::value_type& p_value)->void
				{

					l_last_inserted = _new_node(p_root, p_value);
					++l_count;

				});

				base_type::m_size += l_count;

				return l_last_inserted;
			}

			node_type* _free_node(node_type*& p_root, node_type* p_node)
			{
				node_type* l_result = _node_successor(p_node);

				if(!p_node->m_left && !p_node->m_right)
				{
					_refresh_node_heights(p_root, p_node, -1);

					// If curr node is not root
					if(p_node->m_parent)
					{
						if(p_node->m_parent->m_left == p_node)
							p_node->m_parent->m_left = nullptr;
						else
							p_node->m_parent->m_right = nullptr;

						bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_parent);
					}
					else
						p_root = nullptr;

					bc_allocator_traits< internal_allocator_type >::destroy(m_allocator, p_node);
					bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, p_node);
				}
				else if((p_node->m_left && !p_node->m_right) || (!p_node->m_left && p_node->m_right))
				{
					_refresh_node_heights(p_root, p_node, -1);

					// If curr node is not root
					if(p_node->m_parent)
					{
						if(p_node->m_parent->m_left == p_node)
							if(p_node->m_left)
							{
								p_node->m_parent->m_left = p_node->m_left;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_left);
								p_node->m_left->m_parent = p_node->m_parent;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_left->m_parent);

								bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_left);
							}
							else
							{
								p_node->m_parent->m_left = p_node->m_right;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_left);
								p_node->m_right->m_parent = p_node->m_parent;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_right->m_parent);

								bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_right);
							}
						else
							if (p_node->m_left)
							{
								p_node->m_parent->m_right = p_node->m_left;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_right);
								p_node->m_left->m_parent = p_node->m_parent;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_left->m_parent);

								bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_left);
							}
							else
							{
								p_node->m_parent->m_right = p_node->m_right;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_right);
								p_node->m_right->m_parent = p_node->m_parent;
								bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_right->m_parent);

								bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_right);
							}

							bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_parent);
					}
					else
					{
						if(p_node->m_left)
						{
							p_node->m_left->m_parent = nullptr;
							bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_root);
							p_root = p_node->m_left;
							bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_root);

							bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_left);
						}
						else
						{
							p_node->m_right->m_parent = nullptr;
							bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_root);
							p_root = p_node->m_right;
							bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_root);

							bc_allocator_traits< internal_allocator_type >::unregister_pointer(m_allocator, &p_node->m_right);
						}
					}

					bc_allocator_traits< internal_allocator_type >::destroy(m_allocator, p_node);
					bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, p_node);
				}
				else
				{
					node_type* l_succ = _node_successor(p_node);
					/*p_node->m_value = std::move(l_succ->m_value);
					_free_node(p_root, l_succ);*/
					p_node->m_left->m_parent = l_succ;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_node->m_left->m_parent);
					p_node->m_right->m_parent = l_succ;
					bc_allocator_traits< internal_allocator_type >::register_pointer(m_allocator, &p_node->m_right->m_parent);
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_succ->m_parent);
					l_succ->m_parent = p_node->m_parent;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_succ->m_parent);
					if (p_node->m_parent->m_left == p_node)
					{
						p_node->m_parent->m_left = l_succ;
						bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_left);
					}
					else
					{
						p_node->m_parent->m_right = l_succ;
						bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_right);
					}

					bc_allocator_traits< internal_allocator_type >::destroy(m_allocator, p_node);
					bc_allocator_traits< internal_allocator_type >::deallocate(m_allocator, p_node);
				}

				return l_result;
			};
			
			node_type* _copy_node_and_sub_tree(node_type* p_cpy_node, node_type* p_parent)
			{
				node_type* l_node = bc_allocator_traits< internal_allocator_type >::allocate(m_allocator, 1);
				bc_allocator_traits< internal_allocator_type >::construct(
					m_allocator, 
					l_node, 
					p_cpy_node->m_value);

				l_node->m_left_height = p_cpy_node->m_left_height;
				l_node->m_right_height = p_cpy_node->m_right_height;

				l_node->m_parent = p_parent;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_node->m_parent);
				if (p_cpy_node->m_left)
				{
					l_node->m_left = _copy_node_and_sub_tree(p_cpy_node->m_left, l_node);
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_node->m_left);
				}
				else
					l_node->m_left = nullptr;

				if (p_cpy_node->m_right)
				{
					l_node->m_right = _copy_node_and_sub_tree(p_cpy_node->m_right, l_node);
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_node->m_right);
				}
				else
					l_node->m_right = nullptr;

				return l_node;
			};

			node_type* _node_find(node_type* p_root, const key_type& p_key, bool p_as_lower_bound) const
			{
				node_type* l_prev_node = nullptr;
				node_type* l_curr_node = p_root;

				while(l_curr_node && !(!m_compare(p_key, l_curr_node->m_value.first) && !(m_compare(l_curr_node->m_value.first, p_key))))
				{
					l_prev_node = l_curr_node;

					if(m_compare(p_key, l_curr_node->m_value.first))
						l_curr_node = l_curr_node->m_left;
					else
						l_curr_node = l_curr_node->m_right;
				}

				// doesn't find key, continue to up until we find an element with higher priority(behavior of lower_bound)
				if (!l_curr_node && p_as_lower_bound)
				{
					l_curr_node = l_prev_node;
					if (l_curr_node)
					{
						while (l_curr_node->m_parent && m_compare(l_curr_node->m_parent->m_value.first, p_key))
						{
							l_curr_node = l_curr_node->m_parent;
						}
					}

					l_curr_node = l_curr_node->m_parent;
				}

				return l_curr_node;
			};

			bcInline void _node_rotate_left(node_type*& p_root, node_type* p_node)
			{
				node_type* l_right = p_node->m_right;

				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_right);
				p_node->m_right = l_right->m_left;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_right);
				if (l_right->m_left)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_right->m_left->m_parent);
					l_right->m_left->m_parent = p_node;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_right->m_left->m_parent);
				}

				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_right->m_parent);
				l_right->m_parent = p_node->m_parent;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_right->m_parent);

				if (p_node->m_parent == nullptr)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_root);
					p_root = l_right;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_root);
				}
				else if (p_node == p_node->m_parent->m_left)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_left);
					p_node->m_parent->m_left = l_right;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_left);
				}
				else
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_right);
					p_node->m_parent->m_right = l_right;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent->m_right);
				}
					
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_right->m_left);
				l_right->m_left = p_node;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_right->m_left);
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent);
				p_node->m_parent = l_right;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_parent);
			};
			
			bcInline void _node_rotate_right(node_type*& p_root, node_type* p_node)
			{
				node_type* l_left = p_node->m_left;

				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_left);
				p_node->m_left = l_left->m_right;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &p_node->m_left);
				if (l_left->m_right)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_left->m_right->m_parent);
					l_left->m_right->m_parent = p_node;
					bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_left->m_right->m_parent);
				}

				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_left->m_parent);
				l_left->m_parent = p_node->m_parent;
				bc_allocator_traits<internal_allocator_type>::register_pointer(m_allocator, &l_left->m_parent);

				if (p_node->m_parent == nullptr)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_root);
					p_root = l_left;
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_root);
				}
				else if (p_node == p_node->m_parent->m_left)
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_left);
					p_node->m_parent->m_left = l_left;
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_left);
				}
				else
				{
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_right);
					p_node->m_parent->m_right = l_left;
					bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent->m_right);
				}

				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_left->m_right);
				l_left->m_right = p_node;
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &l_left->m_right);
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent);
				p_node->m_parent = l_left;
				bc_allocator_traits<internal_allocator_type>::unregister_pointer(m_allocator, &p_node->m_parent);
			};

			bcInline static node_type* _node_successor(node_type* p_node)
			{
				node_type* l_curr_node = p_node;
				node_type* l_p_node;

				if(l_curr_node->m_right)
					return _node_min(l_curr_node->m_right);

				l_p_node = l_curr_node->m_parent;

				while(l_p_node && l_curr_node == l_p_node->m_right)
				{
					l_curr_node = l_p_node;
					l_p_node = l_p_node->m_parent;
				}

				return l_p_node;
			};
			
			bcInline static node_type* _node_pre_decessor(node_type* p_node)
			{
				node_type* l_curr_node = p_node;
				node_type* l_p_node;

				if(l_curr_node->m_left)
					return _node_max(l_curr_node->m_left);

				l_p_node = l_curr_node->m_parent;

				while(l_p_node && l_curr_node == l_p_node->m_left)
				{
					l_curr_node = l_p_node;
					l_p_node = l_p_node->m_parent;
				}

				return l_p_node;
			};

			bcInline static node_type* _node_max(node_type* p_node)
			{
				node_type* l_node = p_node;

				if (!l_node)
					return l_node;

				while(l_node->m_right != nullptr)
					l_node = l_node->m_right;

				return l_node;
			};
			
			bcInline static node_type* _node_min(node_type* p_node)
			{
				node_type* l_node = p_node;

				if (!l_node)
					return l_node;

				while(l_node->m_left != nullptr)
					l_node = l_node->m_left;

				return l_node;
			};

			node_pointer m_root;
			key_compare_type m_compare;
			internal_allocator_type m_allocator;
		};

		template < class TK, typename T, bcUINT32 MaxDiff, class TCompare = std::less< TK >, class TAllocator = bc_allocator< std::pair<const TK, T> > >
		class bc_tree : public bc_tree_base< TK, T, MaxDiff, TCompare, TAllocator >
		{
			bc_make_iterators_friend(bc_tree);

		public:
			using this_type = bc_tree;
			using base_type = bc_tree_base< TK, T, MaxDiff, TCompare, TAllocator >;
			using key_type = typename base_type::key_type;
			using mapped_type = typename base_type::mapped_type;
			using key_compare_type = typename base_type::key_compare_type;
			using value_type = typename base_type::value_type;
			using allocator_type = typename base_type::allocator_type;
			using pointer = typename base_type::pointer;
			using const_pointer = typename base_type::const_pointer;
			using reference = typename base_type::reference;
			using const_reference = typename base_type::const_reference;
			using difference_type = typename base_type::difference_type;
			using size_type = typename base_type::size_type;
			using iterator = bc_bidirectional_iterator< this_type >;
			using const_iterator = bc_const_bidirectional_iterator< this_type >;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		protected:
			using node_type = typename base_type::node_type;
			using node_pointer = typename base_type::node_pointer;
			using internal_allocator_type = typename base_type::internal_allocator_type;
		
		public:
			bc_tree();

			explicit bc_tree(const key_compare_type& p_compare, const allocator_type& p_allocator = allocator_type());

			template< class TInputIterator >
			bc_tree(TInputIterator p_first, TInputIterator p_last, const TCompare& p_compare = TCompare(), const TAllocator& p_allocator = TAllocator());

			bc_tree(std::initializer_list<value_type> p_initializer, const TCompare& p_compare = TCompare(), const TAllocator& p_allocator = TAllocator());

			bc_tree(const this_type& p_other);

			bc_tree(const this_type& p_other, const TAllocator& p_allocator);

			bc_tree(this_type&& p_other);

			bc_tree(this_type&& p_other, const TAllocator& p_allocator);

			~bc_tree();

			bc_tree& operator =(const this_type& p_other);
			
			bc_tree& operator =(this_type&& p_other);
			
			bc_tree& operator =(std::initializer_list<value_type> p_initializer);

			allocator_type get_allocator() const;

			mapped_type& at(const key_type& p_key);
			
			const mapped_type& at(const key_type& p_key) const;

			mapped_type& operator[](const key_type& p_key);
			
			mapped_type& operator[](key_type&& p_key);

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

			std::pair<iterator, bool> insert(const value_type& p_value);

			std::pair<iterator, bool> insert(value_type&& p_value);

			iterator insert(const_iterator p_hint, const value_type& p_value);

			iterator insert(const_iterator p_hint, value_type&& p_value);

			template< class TInputIterator >
			void insert(TInputIterator p_first, TInputIterator p_last);

			void insert(std::initializer_list<value_type> p_initializer);

			template< class ...TArgs >
			std::pair<iterator, bool> emplace(TArgs&&... p_args);

			// TODO implement hint function
			template <class ...TArgs>
			iterator emplace_hint(const_iterator p_hint, TArgs&&... p_args);

			iterator erase(const_iterator p_position);

			iterator erase(const_iterator p_first, const_iterator p_last);

			size_type erase(const key_type& p_key);

			void swap(this_type& p_other);

			size_type count(const key_type& p_key) const;

			iterator find(const key_type& p_key);

			const_iterator find(const key_type& p_key) const;

			std::pair<iterator, iterator> equal_range(const key_type& p_key);

			std::pair<const_iterator, const_iterator> equal_range(const key_type& p_key) const;

			iterator lower_bound(const key_type& p_key);

			const_iterator lower_bound(const key_type& p_key) const;

			iterator upper_bound(const key_type& p_key);

			const_iterator upper_bound(const key_type& p_key) const;

			key_compare_type key_comp() const;

			typename base_type::value_compare value_comp() const;

		protected:

		private:
			bcInline void _assign(const this_type& p_other, const allocator_type* p_allocator)
			{
				// Clear content before allocator change
				clear();

				if (p_allocator)
					base_type::m_allocator = *p_allocator;
				else
					base_type::m_allocator = p_other.m_allocator;

				base_type::m_compare = p_other.m_compare;

				base_type::m_root = base_type::_copy_node_and_sub_tree(p_other.m_root, nullptr);
			}

			bcInline void _assign(this_type&& p_other, const allocator_type* p_allocator)
			{
				// Clear content before allocator change
				clear();

				if (p_allocator)
					base_type::m_allocator = *p_allocator;
				else
					base_type::m_allocator = std::move(p_other.m_allocator);

				base_type::m_compare = p_other.m_compare;
				base_type::m_size = p_other.m_size;
				base_type::m_root = p_other.m_root;

				p_other.m_size = 0;
				p_other.m_root = nullptr;
			}

		};

		template< typename TKey,
			typename T,
			bcUINT32 MaxDiff,
			class TCompare,
			template< typename > class TAllocator >
		using bc_tree_a = bc_tree< TKey, T, MaxDiff, TCompare, TAllocator< std::pair< const TKey, T > > >;

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree()
			: bc_tree(key_compare_type())
		{
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(const key_compare_type& p_compare, const allocator_type& p_allocator)
			: bc_tree_base(p_compare, p_allocator)
		{
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		template< class TInputIterator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(TInputIterator p_first, TInputIterator p_last, const TCompare& p_compare, const TAllocator& p_allocator)
			: bc_tree_base(p_compare, p_allocator)
		{
			base_type::_new_node(base_type::m_root, p_first, p_last, std::iterator_traits<TInputIterator>::iterator_category());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(std::initializer_list<value_type> p_initializer, const TCompare& p_compare, const TAllocator& p_allocator)
			: bc_tree_base(p_compare, p_allocator)
		{
			base_type::_new_node(base_type::m_root,
				std::begin(p_initializer),
				std::end(p_initializer),
				std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(const this_type& p_other)
			: bc_tree_base()
		{
			_assign(p_other, nullptr);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(const this_type& p_other, const TAllocator& p_allocator)
		{
			_assign(p_other, &p_allocator);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(this_type&& p_other)
		{
			_assign(std::move(p_other), nullptr);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::bc_tree(this_type&& p_other, const TAllocator& p_allocator)
		{
			_assign(std::move(p_other), &p_allocator);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::~bc_tree()
		{
			clear();
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::operator=(const this_type& p_other)
		{
			_assign(p_other, nullptr);

			return *this;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::operator=(this_type&& p_other)
		{
			_assign(std::move(p_other), nullptr);

			return *this;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		bc_tree<TK, T, MaxDiff, TCompare, TAllocator>& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::operator=(std::initializer_list<value_type> p_initializer)
		{
			clear();
			base_type::_new_node(base_type::m_root,
				std::begin(p_initializer),
				std::end(p_initializer),
				std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category());

			return *this;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::allocator_type bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::get_allocator() const
		{
			return typename bc_allocator_traits< internal_allocator_type >::template rebind_alloc< value_type >::other(base_type::m_allocator);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::mapped_type& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::at(const key_type& p_key)
		{
			return const_cast<mapped_type&>(static_cast<const this_type*>(this)->at(p_key));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		const typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::mapped_type& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::at(const key_type& p_key) const
		{
			node_type* l_node = nullptr;
			bool l_found = base_type::_node_find(base_type::m_root, p_key, &l_node);

			if (!l_found)
				throw std::out_of_range("Value with specified key doesn't exist");

			return l_node->second;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::mapped_type& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::operator[](const key_type& p_key)
		{
			const_iterator l_where = lower_bound(p_key);
			
			if (l_where == end() || base_type::m_compare(p_key, l_where->first))
			{
				l_where = emplace_hint(l_where, p_key, mapped_type());
			}

			return l_where->second;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::mapped_type& bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::operator[](key_type&& p_key)
		{
			const_iterator l_where = lower_bound(p_key);
			
			if (l_where == end() || base_type::m_compare(p_key, l_where->first))
			{
				l_where = emplace_hint(l_where, std::move(p_key), mapped_type());
			}

			return l_where->second;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::begin()
		{
			return iterator(this, base_type::_node_min(base_type::m_root));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::begin() const
		{
			return const_iterator(this, base_type::_node_min(base_type::m_root));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::cbegin() const
		{
			return const_iterator(this, base_type::_node_min(base_type::m_root));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::end()
		{
			return iterator(this, nullptr);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::end() const
		{
			return const_iterator(this, nullptr);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::cend() const
		{
			return const_iterator(this, nullptr);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::rbegin()
		{
			return reverse_iterator(iterator(this, base_type::m_first));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::rbegin() const
		{
			return const_reverse_iterator(const_iterator(this, base_type::m_first));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::crbegin() const
		{
			return const_reverse_iterator(this, base_type::m_first);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::rend()
		{
			return reverse_iterator(this, base_type::_get_end());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::rend() const
		{
			return const_reverse_iterator(this, base_type::_get_end());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_reverse_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::crend() const
		{
			return const_reverse_iterator(this, base_type::_get_end());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		void bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::clear()
		{
			node_type* l_curr_node = base_type::m_root;
			node_type* l_del_node;
			while (l_curr_node)
			{
				if (l_curr_node->m_left)
					l_curr_node = l_curr_node->m_left;
				else if (l_curr_node->m_right)
					l_curr_node = l_curr_node->m_right;
				else
				{
					l_del_node = l_curr_node;
					l_curr_node = l_curr_node->m_parent;
					// If we aren't removing root
					if (l_curr_node)
					{
						if (l_del_node == l_curr_node->m_left)
							l_curr_node->m_left = nullptr;
						else
							l_curr_node->m_right = nullptr;
					}

					bc_allocator_traits< internal_allocator_type >::destroy(base_type::m_allocator, l_del_node);
					bc_allocator_traits< internal_allocator_type >::deallocate(base_type::m_allocator, l_del_node);
				}
			}

			base_type::m_size = 0;
			base_type::m_root = nullptr;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		std::pair<typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator, bool> bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(const value_type& p_value)
		{
			node_type* l_node = nullptr;
			bool l_inserted = base_type::_new_node(base_type::m_root, &l_node, p_value);

			return std::pair<iterator, bool>(iterator(this, l_node), l_inserted);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		std::pair<typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator, bool> bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(value_type&& p_value)
		{
			node_type* l_node = nullptr;
			bool l_inserted = base_type::_new_node(base_type::m_root, &l_node, std::move(p_value));

			return std::pair<iterator, bool>(iterator(this, l_node), l_inserted);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(const_iterator p_hint, const value_type& p_value)
		{
			// TODO use hint
			return insert(p_value).first;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(const_iterator p_hint, value_type&& p_value)
		{
			// TODO use hint
			return insert(std::move(p_value)).first;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		template< class TInputIterator >
		void bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(TInputIterator p_first, TInputIterator p_last)
		{
			base_type::_new_node(base_type::m_root, p_first, p_last, std::iterator_traits<TInputIterator>::iterator_category());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		void bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::insert(std::initializer_list<value_type> p_initializer)
		{
			base_type::_new_node(base_type::m_root,
				std::begin(p_initializer),
				std::end(p_initializer),
				std::iterator_traits< typename std::initializer_list< value_type >::iterator >::iterator_category());
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		template< class ...TArgs >
		std::pair<typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator, bool> bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::emplace(TArgs&&... p_args)
		{
			node_type* l_node = nullptr;
			bool l_inserted = base_type::_new_node(base_type::m_root, &l_node, std::forward<TArgs>(p_args)...);

			return std::pair<iterator, bool>(iterator(this, l_node), l_inserted);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		template< class ... TArgs >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::emplace_hint(const_iterator p_hint, TArgs&&... p_args)
		{
			return emplace(std::forward<TArgs>(p_args)...).first;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::erase(const_iterator p_position)
		{
			node_type* l_node = base_type::_free_node(base_type::m_root, p_position.get_node());

			return iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::erase(const_iterator p_first, const_iterator p_last)
		{
			node_type* l_node = nullptr;

			while (p_first != p_last)
			{
				l_node = erase(p_first++);
			}

			return iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::size_type bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::erase(const key_type& p_key)
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, false);
			
			if (l_node)
			{
				base_type::_free_node(base_type::m_root, l_node);
				return 1;
			}

			return 0;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		void bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::swap(this_type& p_other)
		{
			using std::swap;

			std::swap(base_type::m_size, p_other.m_size);
			swap(base_type::m_allocator, p_other.m_allocator);
			swap(base_type::m_root, p_other.m_root);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::size_type bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::count(const key_type& p_key) const
		{
			return base_type::_node_find(base_type::m_root, p_key, false) != nullptr ? 1 : 0;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::find(const key_type& p_key)
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, false);

			return iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::find(const key_type& p_key) const
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, false);

			return const_iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		std::pair<typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator, typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator> bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::equal_range(const key_type& p_key)
		{
			node_type* l_lower = base_type::_node_find(base_type::m_root, p_key, true);
			node_type* l_upper = l_lower;

			while (!base_type::m_compare(p_key, l_upper->m_value.first) && !base_type::m_compare(l_upper->m_value.first, p_key))
				++l_upper;

			return std::pair<iterator, iterator>(iterator(this, l_lower), iterator(this, l_upper));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		std::pair<typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator, typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator> bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::equal_range(const key_type& p_key) const
		{
			node_type* l_lower = base_type::_node_find(base_type::m_root, p_key, true);
			node_type* l_upper = l_lower;

			while (!base_type::m_compare(p_key, l_upper->m_value.first) && !base_type::m_compare(l_upper->m_value.first, p_key))
				++l_upper;

			return std::pair<iterator, iterator>(iterator(this, l_lower), iterator(this, l_upper));
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::lower_bound(const key_type& p_key)
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, true);

			return iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::lower_bound(const key_type& p_key) const
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, true);

			return const_iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::upper_bound(const key_type& p_key)
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, true);
			if (!base_type::m_compare(l_node->m_value.first, p_key) && !base_type::m_compare(p_key, l_node->m_value.first))
				l_node = base_type::_node_successor(l_node);

			return iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::const_iterator bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::upper_bound(const key_type& p_key) const
		{
			node_type* l_node = base_type::_node_find(base_type::m_root, p_key, true);
			if (!base_type::m_compare(l_node->m_value.first, p_key) && !base_type::m_compare(p_key, l_node->m_value.first))
				l_node = base_type::_node_successor(l_node);

			return const_iterator(this, l_node);
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::key_compare_type bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::key_comp() const
		{
			return base_type::m_compare;
		}

		template< class TK, typename T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		typename bc_tree_base<TK, T, MaxDiff, TCompare, TAllocator>::value_compare bc_tree<TK, T, MaxDiff, TCompare, TAllocator>::value_comp() const
		{
			return base_type::value_compare(base_type::m_compare);
		}

		template< class TKey, class T, bcUINT32 MaxDiff, class TCompare, class TAllocator >
		void swap(bc_tree<TKey, T, MaxDiff, TCompare, TAllocator>& p_first, bc_tree<TKey, T, MaxDiff, TCompare, TAllocator>& p_second)
		{
			p_first.swap(p_second);
		}
	}
}