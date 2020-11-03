// [10/30/2020 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcMainCameraSceneQuery.h"

namespace black_cat
{
	namespace game
	{
		class bc_main_camera_render_state_query : public core::bc_query<core::bc_null_query_context>
		{
			BC_QUERY(rs)

		public:
			explicit bc_main_camera_render_state_query(bc_render_state_buffer p_buffer);

			bc_main_camera_render_state_query(bc_main_camera_render_state_query&&) noexcept;

			~bc_main_camera_render_state_query();

			bc_main_camera_render_state_query& operator=(bc_main_camera_render_state_query&&) noexcept;

			template<class TComponent, typename ...TArgs>
			bc_main_camera_render_state_query& only(TArgs&&... p_render_args) noexcept;

			void execute(const core::bc_null_query_context& p_context) noexcept override;

			bc_render_state_buffer get_render_state_buffer() noexcept;

		private:
			bc_render_state_buffer m_buffer;
			core::bc_delegate<void(const bc_scene_graph_buffer&, bc_render_state_buffer&)> m_execute_with_component;
		};

		inline bc_main_camera_render_state_query::bc_main_camera_render_state_query(bc_render_state_buffer p_buffer)
			: bc_query(message_name()),
			m_buffer(std::move(p_buffer))
		{
		}

		inline bc_main_camera_render_state_query::bc_main_camera_render_state_query(bc_main_camera_render_state_query&& p_other) noexcept
			: bc_query(p_other),
			m_buffer(std::move(p_other.m_buffer)),
			m_execute_with_component(std::move(p_other.m_execute_with_component))
		{
		}

		inline bc_main_camera_render_state_query::~bc_main_camera_render_state_query() = default;

		inline bc_main_camera_render_state_query& bc_main_camera_render_state_query::operator=(bc_main_camera_render_state_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_buffer = std::move(p_other.m_buffer);
			m_execute_with_component = std::move(p_other.m_execute_with_component);
			
			return *this;
		}

		template< class TComponent, typename ... TArgs >
		bc_main_camera_render_state_query& bc_main_camera_render_state_query::only(TArgs&&... p_render_args) noexcept
		{
			auto l_lambda = [=](const bc_scene_graph_buffer& p_scene_buffer, bc_render_state_buffer& p_render_buffer, auto&&... p_render_args)
			{
				p_scene_buffer.render_actors< TComponent >(p_render_buffer, p_render_args...);
			};
			auto l_callable_lambda = std::bind(l_lambda, std::placeholders::_1, std::placeholders::_2, std::forward<TArgs>(p_render_args)...);
			
			m_execute_with_component.bind(l_callable_lambda);

			return *this;
		}

		inline void bc_main_camera_render_state_query::execute(const core::bc_null_query_context & p_context) noexcept
		{
			auto& l_main_camera_scene_buffer = p_context.get_shared_query<bc_main_camera_scene_query>().get_scene_buffer();

			if (m_execute_with_component.is_valid())
			{
				m_execute_with_component(l_main_camera_scene_buffer, m_buffer);
			}
			else
			{
				l_main_camera_scene_buffer.render_actors(m_buffer);
			}
		}

		inline bc_render_state_buffer bc_main_camera_render_state_query::get_render_state_buffer() noexcept
		{
			return std::move(m_buffer);
		}
	}
}