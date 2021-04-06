// [05/30/2016 MRB]

#pragma once

#include "Core/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcNullable.h"

namespace black_cat
{
	namespace core
	{
		/**
		 * \brief Contains parameters from data driven sources, e.g. json files.
		 * \n By default memory is allocated from program stack.
		 */
		class bc_data_driven_parameter
		{
		private:
			using key_hash = std::hash< const bcCHAR* >;
			using map_type = bc_unordered_map_a
			<
				key_hash::result_type,
				bc_any,
				std::hash< key_hash::result_type >,
				std::equal_to< key_hash::result_type >,
				bc_runtime_allocator
			>;

		public:
			bc_data_driven_parameter()
				: bc_data_driven_parameter(bc_alloc_type::program)
			{
			}

			explicit bc_data_driven_parameter(bc_alloc_type p_alloc_type)
				: m_values(map_type::allocator_type(p_alloc_type))
			{
			}

			bc_data_driven_parameter(const bc_data_driven_parameter&) = default;

			bc_data_driven_parameter(bc_data_driven_parameter&&) = default;

			~bc_data_driven_parameter() = default;

			bc_data_driven_parameter& operator=(const bc_data_driven_parameter&) = default;

			bc_data_driven_parameter& operator=(bc_data_driven_parameter&&) = default;

			template< typename T >
			bc_data_driven_parameter& add_value(const bcCHAR* p_name, T&& p_value)
			{
				auto l_hash = key_hash()(p_name);
				bc_any l_parameter;
				l_parameter.set_value(std::forward<T>(p_value));

				m_values.insert(map_type::value_type(l_hash, std::move(l_parameter)));

				return *this;
			}

			bc_data_driven_parameter& add_value(const bcCHAR* p_name, bc_parameter_pack& p_value)
			{
				auto l_hash = key_hash()(p_name);

				m_values.insert(map_type::value_type(l_hash, bc_any(p_value)));

				return *this;
			}

			bc_data_driven_parameter& add_value(const bcCHAR* p_name, bc_any& p_value)
			{
				auto l_hash = key_hash()(p_name);

				m_values.insert(map_type::value_type(l_hash, p_value));

				return *this;
			}

			bc_data_driven_parameter& add_value(const bcCHAR* p_name, bc_any&& p_value)
			{
				auto l_hash = key_hash()(p_name);

				m_values.insert(map_type::value_type(l_hash, std::move(p_value)));

				return *this;
			}

			/**
			 * \brief Return null if parameter is not presented
			 * \tparam T 
			 * \param p_name 
			 * \return 
			 */
			template< typename T >
			const T* get_value(const bcCHAR* p_name) const
			{
				const auto l_hash = key_hash()(p_name);
				const auto l_value = m_values.find(l_hash);

				if (l_value == std::end(m_values))
				{
					return nullptr;
				}

				return l_value->second.as<T>();
			}

			/**
			 * \brief Throw exception if parameter isn't presented
			 * \tparam T 
			 * \param p_name 
			 * \return 
			 */
			template< typename T >
			const T& get_value_throw(const bcCHAR* p_name) const
			{
				const T* l_value = get_value<T>(p_name);

				if (!l_value)
				{
					const auto l_msg = bc_string_frame("No parameter with given name was found: ") + p_name;
					throw bc_key_not_found_exception(l_msg.c_str());
				}

				return *l_value;
			}

			bc_nullable<bc_vector2f> get_value_vector2f(const bcCHAR* p_name) const
			{
				const auto* l_value = get_value<bc_vector<bc_any>>(p_name);
				if (!l_value || l_value->size() != 2)
				{
					return nullptr;
				}

				return bc_nullable<bc_vector2f>
				(
					{
						(*l_value)[0].as_throw<bcFLOAT>(),
						(*l_value)[1].as_throw<bcFLOAT>()
					}
				);
			}

			bc_vector2f get_value_vector2f_throw(const bcCHAR* p_name) const
			{
				const auto l_value = get_value_vector2f(p_name);

				if (!l_value.is_set())
				{
					const auto l_msg = bc_string_frame("No parameter with given name was found: ") + p_name;
					throw bc_key_not_found_exception(l_msg.c_str());
				}

				return *l_value;
			}
			
			bc_nullable<bc_vector3f> get_value_vector3f(const bcCHAR* p_name) const
			{
				const auto* l_value = get_value<bc_vector<bc_any>>(p_name);
				if(!l_value || l_value->size() != 3)
				{
					return nullptr;
				}

				return bc_nullable<bc_vector3f>
				(
					{
						(*l_value)[0].as_throw<bcFLOAT>(),
						(*l_value)[1].as_throw<bcFLOAT>(),
						(*l_value)[2].as_throw<bcFLOAT>()
					}
				);
			}

			bc_vector3f get_value_vector3f_throw(const bcCHAR* p_name) const
			{
				const auto l_value = get_value_vector3f(p_name);

				if(!l_value.is_set())
				{
					const auto l_msg = bc_string_frame("No parameter with given name was found: ") + p_name;
					throw bc_key_not_found_exception(l_msg.c_str());
				}

				return *l_value;
			}

			bc_nullable<bc_vector4f> get_value_vector4f(const bcCHAR* p_name) const
			{
				const auto* l_value = get_value<bc_vector<bc_any>>(p_name);
				if (!l_value || l_value->size() != 4)
				{
					return nullptr;
				}

				return bc_nullable<bc_vector4f>
				(
					{
						(*l_value)[0].as_throw<bcFLOAT>(),
						(*l_value)[1].as_throw<bcFLOAT>(),
						(*l_value)[2].as_throw<bcFLOAT>(),
						(*l_value)[3].as_throw<bcFLOAT>()
					}
				);
			}

			bc_vector4f get_value_vector4f_throw(const bcCHAR* p_name) const
			{
				const auto l_value = get_value_vector4f(p_name);

				if (!l_value.is_set())
				{
					const auto l_msg = bc_string_frame("No parameter with given name was found: ") + p_name;
					throw bc_key_not_found_exception(l_msg.c_str());
				}

				return *l_value;
			}
			
			void reset()
			{
				m_values.clear();
			}

		private:
			map_type m_values;
		};
	}
}