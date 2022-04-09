// [02/18/2021 MRB]

#pragma once

#include "Core/Utility/bcDelegate.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Query/bcQueryContext.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sq)
			
		public:
			bc_scene_query() noexcept;

			template<typename TCallable>
			explicit bc_scene_query(TCallable p_callable) noexcept;

			bc_scene_query(bc_scene_query&&) = default;

			~bc_scene_query() override = default;

			bc_scene_query& operator=(bc_scene_query&&) = default;

			core::bc_any& get_result() noexcept;
			
			/**
			 * \brief Callable object which will be executed with an instance of bc_scene_query_context
			 * \tparam TCallable Callable object with signature bc_any(const bc_scene_query_context&).
			 * Callable is responsible to check if scene has value.
			 * \param p_callable 
			 */
			template<typename TCallable>
			bc_scene_query& with_callable(TCallable p_callable) noexcept;
		
		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_delegate<core::bc_any(const bc_scene_query_context&)> m_delegate;
			core::bc_any m_result;
		};

		inline bc_scene_query::bc_scene_query() noexcept
			: bc_query(message_name())
		{
		}

		inline core::bc_any& bc_scene_query::get_result() noexcept
		{
			return m_result;
		}

		template<typename TCallable>
		bc_scene_query::bc_scene_query(TCallable p_callable) noexcept
			: bc_query(message_name()),
			m_delegate(std::move(p_callable))
		{
		}

		template< typename TCallable >
		bc_scene_query& bc_scene_query::with_callable(TCallable p_callable) noexcept
		{
			m_delegate.bind(p_callable);
			return *this;
		}

		inline void bc_scene_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			m_result = m_delegate(p_context);
		}
	}	
}