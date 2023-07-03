// [13/12/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcSceneGraphQuery.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_wind_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sc_wnd)
			
		public:
			bc_scene_wind_query();

			bc_scene_wind_query(bc_scene_wind_query&&) noexcept;

			~bc_scene_wind_query() override;
			
			bc_scene_wind_query& operator=(bc_scene_wind_query&&) noexcept;

			core::bc_vector<bc_wind> get_winds() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_vector<bc_wind> m_winds;
		};

		inline bc_scene_wind_query::bc_scene_wind_query()
			: bc_query(get_message_name())
		{
		}

		inline bc_scene_wind_query::bc_scene_wind_query(bc_scene_wind_query&&) noexcept = default;

		inline bc_scene_wind_query::~bc_scene_wind_query() = default;

		inline bc_scene_wind_query& bc_scene_wind_query::operator=(bc_scene_wind_query&&) noexcept = default;

		inline core::bc_vector<bc_wind> bc_scene_wind_query::get_winds() noexcept
		{
			return std::move(m_winds);
		}

		inline void bc_scene_wind_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			if (!p_context.m_scene)
			{
				return;
			}

			bc_scene_graph_query l_scene_graph_query;
			l_scene_graph_query.only<bc_wind_component>().execute(p_context);

			bc_scene_graph_buffer l_scene_graph_buffer = l_scene_graph_query.get_scene_buffer();
			m_winds.reserve(l_scene_graph_buffer.size());
			
			std::transform(std::begin(l_scene_graph_buffer), std::end(l_scene_graph_buffer), std::back_inserter(m_winds), [](const bc_actor& p_actor)
			{
				return *p_actor.get_component<bc_wind_component>()->get_wind();
			});
		}
	}
}