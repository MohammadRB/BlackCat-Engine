// [05/08/2020 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/System/Render/bcShapeDrawer.h"
#include "Game/System/Input/bcCameraFrustum.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_query_context : public core::bc_query_context
		{
		public:
			const bc_scene* m_scene;
		};

		class bc_scene_graph_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(sc_gph)

		public:
			bc_scene_graph_query() noexcept;

			bc_scene_graph_query(bc_scene_graph_query&&) noexcept;

			~bc_scene_graph_query();

			bc_scene_graph_query& operator=(bc_scene_graph_query&&) noexcept;
			
			bc_scene_graph_query& with(const bc_camera_frustum& p_frustum);
			
			template<class TComponent>
			bc_scene_graph_query& only() noexcept;
			
			bc_scene_graph_buffer get_scene_buffer() noexcept;
			
			void execute(const bc_scene_query_context& p_context) noexcept override;
			
		private:
			template<class TComponent>
			static void _execute(bc_scene_graph_query& p_this, const bc_scene_query_context& p_context) noexcept;
			
			core::bc_unique_ptr<bc_camera_frustum> m_frustum;
			bc_scene_graph_buffer m_scene_buffer;
			core::bc_delegate<void(bc_scene_graph_query&, const bc_scene_query_context&)> m_execute_with_component;
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

		inline bc_scene_graph_query& bc_scene_graph_query::operator=(bc_scene_graph_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_frustum = std::move(p_other.m_frustum);
			m_scene_buffer = std::move(p_other.m_scene_buffer);
			m_execute_with_component = std::move(p_other.m_execute_with_component);
			
			return *this;
		}

		inline bc_scene_graph_query& bc_scene_graph_query::with(const bc_camera_frustum& p_frustum)
		{
			m_frustum = core::bc_make_unique<bc_camera_frustum>(p_frustum);
			return *this;
		}

		template< class TComponent >
		bc_scene_graph_query& bc_scene_graph_query::only() noexcept
		{
			m_execute_with_component.bind(&_execute<TComponent>);
			return *this;
		}
		
		inline bc_scene_graph_buffer bc_scene_graph_query::get_scene_buffer() noexcept
		{
			return std::move(m_scene_buffer);
		}
		
		inline void bc_scene_graph_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			if(m_execute_with_component.is_valid())
			{
				m_execute_with_component(*this, p_context);
			}
			else
			{
				if (m_frustum)
				{
					m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors(*m_frustum);
				}
			}
		}

		template< class TComponent >
		void bc_scene_graph_query::_execute(bc_scene_graph_query& p_this, const bc_scene_query_context& p_context) noexcept
		{
			if(p_this.m_frustum)
			{
				p_this.m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors<TComponent>(*p_this.m_frustum);
			}
			else
			{
				p_this.m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors<TComponent>();
			}
		}
	}
}