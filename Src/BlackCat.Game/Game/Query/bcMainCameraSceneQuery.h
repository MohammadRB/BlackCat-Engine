// [10/30/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		class bc_main_camera_scene_query : public core::bc_query< bc_scene_query_context >
		{
			BC_SHARED_QUERY(mca)

		public:
			bc_main_camera_scene_query(const bc_camera_frustum& p_frustum);

			bc_main_camera_scene_query(bc_main_camera_scene_query&&) noexcept;

			~bc_main_camera_scene_query();

			bc_main_camera_scene_query& operator=(bc_main_camera_scene_query&&) noexcept;

			const bc_scene_graph_buffer& get_scene_buffer() const noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			bc_camera_frustum m_frustum;
			bc_scene_graph_buffer m_scene_buffer;
		};

		inline bc_main_camera_scene_query::bc_main_camera_scene_query(const bc_camera_frustum& p_frustum)
			: bc_query(message_name()),
			m_frustum(p_frustum)
		{
		}

		inline bc_main_camera_scene_query::bc_main_camera_scene_query(bc_main_camera_scene_query&& p_other) noexcept
			: bc_query(p_other),
			m_frustum(p_other.m_frustum),
			m_scene_buffer(std::move(p_other.m_scene_buffer))
		{
		}

		inline bc_main_camera_scene_query::~bc_main_camera_scene_query() = default;

		inline bc_main_camera_scene_query& bc_main_camera_scene_query::operator=(bc_main_camera_scene_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_frustum = std::move(p_other.m_frustum);
			m_scene_buffer = std::move(p_other.m_scene_buffer);
			
			return *this;
		}

		inline const bc_scene_graph_buffer& bc_main_camera_scene_query::get_scene_buffer() const noexcept
		{
			return m_scene_buffer;
		}

		inline void bc_main_camera_scene_query::execute(const bc_scene_query_context & p_context) noexcept
		{
			m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors(m_frustum);
		}
	}
}