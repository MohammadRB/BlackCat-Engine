// [10/30/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_main_camera_scene_shared_query : public core::bc_query<bc_scene_query_context>
		{
			BC_SHARED_QUERY(mca)

		public:
			explicit bc_main_camera_scene_shared_query(const bc_camera_frustum& p_frustum);

			bc_main_camera_scene_shared_query(bc_main_camera_scene_shared_query&&) noexcept;

			~bc_main_camera_scene_shared_query() override;

			bc_main_camera_scene_shared_query& operator=(bc_main_camera_scene_shared_query&&) noexcept;

			bc_main_camera_scene_shared_query& with_sort(const core::bc_vector3f& p_camera_position) noexcept;

			const bc_scene_graph_buffer& get_scene_buffer() const noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;

		private:
			core::bc_nullable<core::bc_vector3f> m_camera_position;
			bc_camera_frustum m_frustum;
			bc_scene_graph_buffer m_scene_buffer;
		};

		inline bc_main_camera_scene_shared_query::bc_main_camera_scene_shared_query(const bc_camera_frustum& p_frustum)
			: bc_query(message_name()),
			m_frustum(p_frustum)
		{
		}

		inline bc_main_camera_scene_shared_query::bc_main_camera_scene_shared_query(bc_main_camera_scene_shared_query&& p_other) noexcept
			: bc_query(p_other),
			m_camera_position(p_other.m_camera_position),
			m_frustum(p_other.m_frustum),
			m_scene_buffer(std::move(p_other.m_scene_buffer))
		{
		}

		inline bc_main_camera_scene_shared_query::~bc_main_camera_scene_shared_query() = default;

		inline bc_main_camera_scene_shared_query& bc_main_camera_scene_shared_query::operator=(bc_main_camera_scene_shared_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_camera_position = p_other.m_camera_position;
			m_frustum = std::move(p_other.m_frustum);
			m_scene_buffer = std::move(p_other.m_scene_buffer);
			
			return *this;
		}

		inline bc_main_camera_scene_shared_query& bc_main_camera_scene_shared_query::with_sort(const core::bc_vector3f& p_camera_position) noexcept
		{
			m_camera_position.reset(p_camera_position);
			return *this;
		}

		inline const bc_scene_graph_buffer& bc_main_camera_scene_shared_query::get_scene_buffer() const noexcept
		{
			return m_scene_buffer;
		}

		inline void bc_main_camera_scene_shared_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			if (!p_context.m_scene)
			{
				return;
			}

			m_scene_buffer = p_context.m_scene->get_scene_graph().get_actors(m_frustum);

			if(m_camera_position.has_value())
			{
				core::bc_vector_frame<std::pair<bc_actor, bcFLOAT>> l_actors;
				l_actors.reserve(m_scene_buffer.size());

				std::transform
				(
					std::begin(m_scene_buffer),
					std::end(m_scene_buffer),
					std::back_inserter(l_actors),
					[this](const bc_actor& p_actor)
					{
						const auto l_position = p_actor.get_component<bc_mediate_component>()->get_position();
						const auto l_distance = (*m_camera_position - l_position).magnitude();

						return std::make_pair(p_actor, l_distance);
					}
				);

				std::sort
				(
					std::begin(l_actors),
					std::end(l_actors),
					[](const auto& p_first, const auto& p_second)
					{
						return p_first.second < p_second.second;
					}
				);

				std::transform
				(
					std::begin(l_actors),
					std::end(l_actors),
					std::begin(m_scene_buffer),
					[](const auto p_entry)
					{
						return p_entry.first;
					}
				);
			}
		}
	}
}