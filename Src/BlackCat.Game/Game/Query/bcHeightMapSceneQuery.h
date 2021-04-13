// [10/30/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/SceneGraph/bcSceneGraphBuffer.h"
#include "Game/Object/Mesh/bcHeightMap.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/Query/bcSceneGraphQuery.h"

namespace black_cat
{
	namespace game
	{
		class bc_height_map_scene_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(hms)
			
		public:
			bc_height_map_scene_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_scene_query);

			bc_height_map_scene_query(bc_height_map_scene_query&&) noexcept;

			~bc_height_map_scene_query();

			bc_height_map_scene_query& operator=(bc_height_map_scene_query&&) noexcept;

			bc_render_state_buffer get_render_state_buffer() noexcept;

			core::bc_vector<bc_height_map_ptr> get_height_maps() noexcept;

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override;
			
		private:
			bc_scene_graph_query m_scene_query;
			bc_actor_render_camera m_camera;
			bc_render_state_buffer m_render_buffer;
			core::bc_vector<bc_height_map_ptr> m_height_maps;
		};

		inline bc_height_map_scene_query::bc_height_map_scene_query(const bc_actor_render_camera& p_camera, bc_render_state_buffer p_scene_query)
			: bc_query(message_name()),
			m_camera(p_camera),
			m_render_buffer(std::move(p_scene_query))
		{
		}

		inline bc_height_map_scene_query::bc_height_map_scene_query(bc_height_map_scene_query&& p_other) noexcept
			: bc_query(p_other),
			m_scene_query(std::move(p_other.m_scene_query)),
			m_camera(p_other.m_camera),
			m_render_buffer(std::move(p_other.m_render_buffer)),
			m_height_maps(std::move(p_other.m_height_maps))
		{
		}

		inline bc_height_map_scene_query::~bc_height_map_scene_query() = default;

		inline bc_height_map_scene_query& bc_height_map_scene_query::operator=(bc_height_map_scene_query&& p_other) noexcept
		{
			bc_query::operator=(p_other);
			m_scene_query = std::move(p_other.m_scene_query);
			m_camera = p_other.m_camera;
			m_render_buffer = std::move(p_other.m_render_buffer);
			m_height_maps = std::move(p_other.m_height_maps);

			return *this;
		}

		inline bc_render_state_buffer bc_height_map_scene_query::get_render_state_buffer() noexcept
		{
			return std::move(m_render_buffer);
		}

		inline core::bc_vector<bc_height_map_ptr> bc_height_map_scene_query::get_height_maps() noexcept
		{
			return std::move(m_height_maps);
		}

		inline void bc_height_map_scene_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			m_scene_query.only<bc_height_map_component>();
			m_scene_query.execute(p_context);

			auto l_scene_buffer = m_scene_query.get_scene_buffer();
			m_height_maps.reserve(l_scene_buffer.size());
			
			for (auto& l_actor : l_scene_buffer)
			{
				auto l_height_map = l_actor.get_component< bc_height_map_component >()->get_height_map_ptr();
				m_height_maps.push_back(l_height_map);
			}

			l_scene_buffer.render_actors<bc_height_map_component>(m_camera, m_render_buffer);
		}
	}
}