// [10/30/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcSceneGraphQuery.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_graph_render_state_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sgrs)

		public:
			bc_scene_graph_render_state_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_render_buffer) noexcept;

			bc_scene_graph_render_state_query(bc_scene_graph_render_state_query&&) noexcept;

			~bc_scene_graph_render_state_query() override;

			bc_scene_graph_render_state_query& operator=(bc_scene_graph_render_state_query&&) noexcept;

			bc_render_state_buffer get_render_state_buffer() noexcept;
			
			bc_scene_graph_render_state_query& with(const bc_camera_frustum& p_frustum);

			template<class TComponent, typename ...TArgs>
			bc_scene_graph_render_state_query& only(TArgs&&... p_render_args) noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			bc_scene_graph_query m_scene_query;
			bc_actor_render_camera m_camera;
			bc_render_state_buffer m_render_buffer;
			core::bc_delegate<void(const bc_scene_graph_buffer&, const bc_actor_render_camera&, bc_render_state_buffer&)> m_execute_with_component;
		};

		inline bc_scene_graph_render_state_query::bc_scene_graph_render_state_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_render_buffer) noexcept
			: bc_query(message_name()),
			m_camera(p_camera),
			m_render_buffer(std::move(p_render_buffer))
		{
		}

		inline bc_scene_graph_render_state_query::bc_scene_graph_render_state_query(bc_scene_graph_render_state_query&& p_other) noexcept
			: bc_query(p_other),
			m_scene_query(std::move(p_other.m_scene_query)),
			m_camera(p_other.m_camera),
			m_render_buffer(std::move(p_other.m_render_buffer)),
			m_execute_with_component(std::move(p_other.m_execute_with_component))
		{
		}

		inline bc_scene_graph_render_state_query::~bc_scene_graph_render_state_query() = default;

		inline bc_scene_graph_render_state_query& bc_scene_graph_render_state_query::operator=(bc_scene_graph_render_state_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_scene_query = std::move(p_other.m_scene_query);
			m_camera = p_other.m_camera;
			m_render_buffer = std::move(p_other.m_render_buffer);
			m_execute_with_component = std::move(p_other.m_execute_with_component);

			return *this;
		}

		inline bc_render_state_buffer bc_scene_graph_render_state_query::get_render_state_buffer() noexcept
		{
			return std::move(m_render_buffer);
		}
		
		inline bc_scene_graph_render_state_query& bc_scene_graph_render_state_query::with(const bc_camera_frustum& p_frustum)
		{
			m_scene_query.with(p_frustum);
			return *this;
		}

		template<class TComponent, typename ...TArgs>
		bc_scene_graph_render_state_query& bc_scene_graph_render_state_query::only(TArgs&&... p_render_args) noexcept
		{
			auto l_lambda = [=](const bc_scene_graph_buffer& p_scene_buffer, const bc_actor_render_camera& p_camera, bc_render_state_buffer& p_render_buffer, auto&&... p_render_args)
			{
				p_scene_buffer.render_actors<TComponent>(p_camera, p_render_buffer, p_render_args...);
			};
			auto l_callable_lambda = std::bind(l_lambda, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::forward<TArgs>(p_render_args)...);
			
			m_scene_query.only<TComponent>();
			m_execute_with_component.bind(l_callable_lambda);
			
			return *this;
		}

		inline void bc_scene_graph_render_state_query::execute(const bc_scene_query_context & p_context) noexcept
		{
			if (!p_context.m_scene)
			{
				return;
			}

			m_scene_query.execute(p_context);
			const auto l_scene_buffer = m_scene_query.get_scene_buffer();

			if (m_execute_with_component.is_valid())
			{
				m_execute_with_component(l_scene_buffer, m_camera, m_render_buffer);
			}
			else
			{
				l_scene_buffer.render_actors(m_camera, m_render_buffer);
			}
		}
	}
}