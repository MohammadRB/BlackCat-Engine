// [30/08/2016 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace game
	{
		using bc_render_pass_variable_t = const bcCHAR*;
		
		class bc_render_pass_resource_manager
		{
		private:
			using map_type = core::bc_unordered_map<bc_render_pass_variable_t, core::bc_any>;

		public:
			bc_render_pass_resource_manager() = default;

			bc_render_pass_resource_manager(bc_render_pass_resource_manager&&) = default;

			~bc_render_pass_resource_manager() = default;

			bc_render_pass_resource_manager& operator=(bc_render_pass_resource_manager&&) = default;

			template<typename T>
			void share_resource(bc_render_pass_variable_t p_variable, T&& p_value)
			{
				m_variables[p_variable] = core::bc_any(std::forward<T>(p_value));
			}

			void unshare_resource(bc_render_pass_variable_t p_variable)
			{
				m_variables.erase(p_variable);
			}

			/**
			 * \brief Return shared variable ar null in case of no variable were found.
			 * \tparam T
			 * \param p_variable
			 * \return
			 */
			template<typename T>
			T* get_resource(bc_render_pass_variable_t p_variable) noexcept
			{
				const auto l_item = m_variables.find(p_variable);

				if (l_item == std::end(m_variables))
				{
					return nullptr;
				}

				return l_item->second.as<T>();
			}

			/**
			 * \brief Return shared variable ar null in case of no variable were found.
			 * \tparam T 
			 * \param p_variable 
			 * \return 
			 */
			template<typename T>
			const T* get_resource(bc_render_pass_variable_t p_variable) const noexcept
			{
				return const_cast<bc_render_pass_resource_manager&>(*this).get_resource<T>(p_variable);
			}

		private:
			map_type m_variables;
		};
	}
}