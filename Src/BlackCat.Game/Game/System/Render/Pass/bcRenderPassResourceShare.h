// [08/30/2016 MRB]

#pragma once

#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcParameterPack.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_render_pass_resource_variable
		{
			actor_list,
			depth_stencil_texture,
			depth_stencil_view,
			back_buffer_view,
			render_target_texture_1,
			render_target_view_1,
			render_target_texture_2,
			render_target_view_2,
			render_target_texture_3,
			render_target_view_3,
			render_target_texture_4,
			render_target_view_4,
			intermediate_texture_1,
			intermediate_texture_view_1,
			intermediate_texture_2,
			intermediate_texture_view_2,
			intermediate_texture_3,
			intermediate_texture_view_3,
			intermediate_texture_4,
			intermediate_texture_view_4
		};

		class bc_render_pass_resource_share
		{
		private:
			using map_type = core::bc_unordered_map< bc_render_pass_resource_variable, core::bc_any >;

		public:
			bc_render_pass_resource_share() = default;

			bc_render_pass_resource_share(bc_render_pass_resource_share&&) = default;

			~bc_render_pass_resource_share() = default;

			bc_render_pass_resource_share& operator=(bc_render_pass_resource_share&&) = default;

			template< typename T >
			void share_resource(bc_render_pass_resource_variable p_variable, T&& p_value)
			{
				m_variables[p_variable] = core::bc_any(std::forward< T >(p_value));
			}

			void unshare_resource(bc_render_pass_resource_variable p_variable)
			{
				m_variables.erase(p_variable);
			}

			/**
			 * \brief Return shared variable ar null in case of no variable were found.
			 * \tparam T 
			 * \param p_variable 
			 * \return 
			 */
			template< typename T >
			T* get_resource(bc_render_pass_resource_variable p_variable) const noexcept
			{
				auto l_item = const_cast< map_type& >(m_variables).find(p_variable);

				if(l_item == std::end(m_variables))
				{
					return nullptr;
				}

				return l_item->second.as< T >();
			}

		protected:

		private:
			map_type m_variables;
		};
	}
}