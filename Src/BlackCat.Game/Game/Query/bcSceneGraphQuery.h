// [10/30/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_graph_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sgq)

		public:
			bc_scene_graph_query() noexcept;

			bc_scene_graph_query(bc_scene_graph_query&&) noexcept;

			~bc_scene_graph_query();

			bc_scene_graph_query& operator=(bc_scene_graph_query&&) noexcept;

			bc_scene_graph_buffer get_scene_buffer() noexcept;
			
			bc_scene_graph_query& with(const bc_camera_frustum& p_frustum);

			template<class TComponent>
			bc_scene_graph_query& only() noexcept;

			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_nullable<bc_camera_frustum> m_frustum;
			core::bc_delegate<bc_scene_graph_buffer(const bc_scene_query_context&, const core::bc_nullable<bc_camera_frustum>&)> m_execute_with_component;
			bc_scene_graph_buffer m_scene_buffer;
		};

		inline bc_scene_graph_query::bc_scene_graph_query() noexcept
			: bc_query(message_name())
		{
		}

		inline bc_scene_graph_query::bc_scene_graph_query(bc_scene_graph_query&& p_other) noexcept
			: bc_query(p_other)
		{
			operator=(std::move(p_other));
		}

		inline bc_scene_graph_query::~bc_scene_graph_query() = default;

		inline bc_scene_graph_query& bc_scene_graph_query::operator=(bc_scene_graph_query && p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_frustum = std::move(p_other.m_frustum);
			m_scene_buffer = std::move(p_other.m_scene_buffer);
			m_execute_with_component = std::move(p_other.m_execute_with_component);

			return *this;
		}

		inline bc_scene_graph_buffer bc_scene_graph_query::get_scene_buffer() noexcept
		{
			return std::move(m_scene_buffer);
		}
		
		inline bc_scene_graph_query& bc_scene_graph_query::with(const bc_camera_frustum & p_frustum)
		{
			m_frustum = p_frustum;
			return *this;
		}

		template< class TComponent >
		bc_scene_graph_query& bc_scene_graph_query::only() noexcept
		{
			m_execute_with_component.bind
			(
				[&](const bc_scene_query_context& p_context, const core::bc_nullable< bc_camera_frustum >& p_frustum)
				{
					if (p_frustum.is_set())
					{
						return p_context.m_scene->get_scene_graph().get_actors< TComponent >(*p_frustum);
					}
					else
					{
						return p_context.m_scene->get_scene_graph().get_actors< TComponent >();
					}
				}
			);
			return *this;
		}

		inline void bc_scene_graph_query::execute(const bc_scene_query_context & p_context) noexcept
		{
			if (m_execute_with_component.is_valid())
			{
				m_scene_buffer = m_execute_with_component(p_context, m_frustum);
			}
			else
			{
				if (m_frustum.is_set())
				{
					m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors(*m_frustum);
				}
			}
		}
	}
}