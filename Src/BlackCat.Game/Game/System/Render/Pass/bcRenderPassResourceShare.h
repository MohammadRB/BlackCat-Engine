// [08/30/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_pass_resource_share
		{
		private:
			using resource_variable_hash = std::hash< constant::bc_render_pass_variable_t >;
			using map_type = core::bc_unordered_map< resource_variable_hash::result_type, core::bc_any >;

		public:
			bc_render_pass_resource_share() = default;

			bc_render_pass_resource_share(bc_render_pass_resource_share&&) = default;

			~bc_render_pass_resource_share() = default;

			bc_render_pass_resource_share& operator=(bc_render_pass_resource_share&&) = default;

			template< typename T >
			void share_resource(constant::bc_render_pass_variable_t p_variable, T&& p_value)
			{
				const auto l_hash = resource_variable_hash()(p_variable);
				m_variables[l_hash] = core::bc_any(std::forward< T >(p_value));
			}

			void unshare_resource(constant::bc_render_pass_variable_t p_variable)
			{
				const auto l_hash = resource_variable_hash()(p_variable);
				m_variables.erase(l_hash);
			}

			/**
			 * \brief Return shared variable ar null in case of no variable were found.
			 * \tparam T
			 * \param p_variable
			 * \return
			 */
			template< typename T >
			T* get_resource(constant::bc_render_pass_variable_t p_variable) noexcept
			{
				const auto l_hash = resource_variable_hash()(p_variable);
				auto l_item = m_variables.find(l_hash);

				if (l_item == std::end(m_variables))
				{
					return nullptr;
				}

				return l_item->second.as< T >();
			}

			/**
			 * \brief Return shared variable ar null in case of no variable were found.
			 * \tparam T 
			 * \param p_variable 
			 * \return 
			 */
			template< typename T >
			const T* get_resource(constant::bc_render_pass_variable_t p_variable) const noexcept
			{
				return const_cast<bc_render_pass_resource_share&>(*this).get_resource<T>(p_variable);
			}

		protected:

		private:
			map_type m_variables;
		};
	}
}