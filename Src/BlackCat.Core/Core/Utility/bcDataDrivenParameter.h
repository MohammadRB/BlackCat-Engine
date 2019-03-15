// [05/30/2016 MRB]

#pragma once

#include "Core/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace core
	{
		/**
		 * \brief This class contains parameters from data driven sources, eg. json files, which these parameters are constant throughout program lifetime.
		 */
		class bc_data_driven_parameter
		{
		private:
			using key_hash = std::hash< const bcCHAR* >;
			using map_type = bc_unordered_map_program< key_hash::result_type, bc_any >;

		public:
			bc_data_driven_parameter() = default;

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
			T* get_value(const bcCHAR* p_name)
			{
				const auto l_hash = key_hash()(p_name);
				const auto l_value = m_values.find(l_hash);

				if (l_value == std::end(m_values))
				{
					return nullptr;
				}

				return l_value->second.as<T>();
			}

			template< typename T >
			const T* get_value(const bcCHAR* p_name) const
			{
				return const_cast<bc_data_driven_parameter*>(this)->get_value<T>(p_name);
			}

			/**
			 * \brief Throw exception if parameter isn't presented
			 * \tparam T 
			 * \param p_name 
			 * \return 
			 */
			template< typename T >
			T& get_value_throw(const bcCHAR* p_name)
			{
				T* l_value = get_value<T>(p_name);

				if (!l_value)
				{
					bc_string_frame l_msg = bc_string_frame("No parameter with given name was found: ") + p_name;
					throw bc_key_not_found_exception(l_msg.c_str());
				}

				return *l_value;
			}

			template< typename T >
			const T& get_value_throw(const bcCHAR* p_name) const
			{
				return const_cast<bc_data_driven_parameter*>(this)->get_value_throw<T>(p_name);
			}

			void reset()
			{
				m_values.clear();
			}

		protected:

		private:
			map_type m_values;
		};
	}
}