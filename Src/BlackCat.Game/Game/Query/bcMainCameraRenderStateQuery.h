// [10/30/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcMainCameraSceneSharedQuery.h"

namespace black_cat
{
	namespace game
	{
		class bc_main_camera_render_state_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(rs)

		public:
			bc_main_camera_render_state_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_buffer);

			bc_main_camera_render_state_query(bc_main_camera_render_state_query&&) noexcept;

			~bc_main_camera_render_state_query() override;

			bc_main_camera_render_state_query& operator=(bc_main_camera_render_state_query&&) noexcept;

			bc_render_state_buffer get_render_state_buffer() noexcept;
			
			template<class TComponent, typename ...TArgs>
			bc_main_camera_render_state_query& only(TArgs&&... p_render_args) noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			bc_actor_render_camera m_camera;
			bc_render_state_buffer m_buffer;
			core::bc_delegate<void(const bc_scene_graph_buffer&, const bc_actor_render_camera&, bc_render_state_buffer&)> m_execute_with_component;
		};

		inline bc_main_camera_render_state_query::bc_main_camera_render_state_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_buffer)
			: bc_query(message_name()),
			m_camera(p_camera),
			m_buffer(std::move(p_buffer))
		{
		}

		inline bc_main_camera_render_state_query::bc_main_camera_render_state_query(bc_main_camera_render_state_query&& p_other) noexcept
			: bc_query(p_other),
			m_camera(p_other.m_camera),
			m_buffer(std::move(p_other.m_buffer)),
			m_execute_with_component(std::move(p_other.m_execute_with_component))
		{
		}

		inline bc_main_camera_render_state_query::~bc_main_camera_render_state_query() = default;

		inline bc_main_camera_render_state_query& bc_main_camera_render_state_query::operator=(bc_main_camera_render_state_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_camera = p_other.m_camera;
			m_buffer = std::move(p_other.m_buffer);
			m_execute_with_component = std::move(p_other.m_execute_with_component);
			
			return *this;
		}

		inline bc_render_state_buffer bc_main_camera_render_state_query::get_render_state_buffer() noexcept
		{
			return std::move(m_buffer);
		}
		
		template<class TComponent, typename ... TArgs>
		bc_main_camera_render_state_query& bc_main_camera_render_state_query::only(TArgs&&... p_render_args) noexcept
		{
			auto l_lambda = [=](const bc_scene_graph_buffer& p_scene_buffer, const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_render_buffer, auto&&... p_render_args)
			{
				p_scene_buffer.render_actors< TComponent >(p_camera, p_render_buffer, p_render_args...);
			};
			auto l_callable_lambda = std::bind(l_lambda, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::forward<TArgs>(p_render_args)...);
			
			m_execute_with_component.bind(l_callable_lambda);

			return *this;
		}

		inline void bc_main_camera_render_state_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			if (!p_context.m_scene)
			{
				return;
			}

			const auto& l_main_camera_scene_buffer = p_context.get_shared_query<bc_main_camera_scene_shared_query>().get_scene_buffer();

			if (m_execute_with_component.is_valid())
			{
				m_execute_with_component(l_main_camera_scene_buffer, m_camera, m_buffer);
			}
			else
			{
				l_main_camera_scene_buffer.render_actors(m_camera, m_buffer);
			}
		}
	}
}