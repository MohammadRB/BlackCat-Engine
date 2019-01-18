// [07/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene_graph::bc_scene_graph(core::bc_unique_ptr<bc_iscene_graph_node> p_scene_graph)
			: m_graph_node(std::move(p_scene_graph))
		{
		}

		bc_scene_graph::~bc_scene_graph()
		{
			if(m_graph_node)
			{
				clear();
			}
		}

		bc_scene_graph::iterator bc_scene_graph::begin() noexcept
		{
			return m_graph_node->begin();
		}

		bc_scene_graph::const_iterator bc_scene_graph::begin() const noexcept
		{
			return m_graph_node->begin();
		}

		bc_scene_graph::const_iterator bc_scene_graph::cbegin() const noexcept
		{
			return m_graph_node->cbegin();
		}

		bc_scene_graph::iterator bc_scene_graph::end() noexcept
		{
			return m_graph_node->end();
		}

		bc_scene_graph::const_iterator bc_scene_graph::end() const noexcept
		{
			return m_graph_node->end();
		}

		bc_scene_graph::const_iterator bc_scene_graph::cend() const noexcept
		{
			return m_graph_node->cend();
		}

		bool bc_scene_graph::add_actor(bc_actor& p_actor)
		{
			return m_graph_node->add_actor(p_actor);
		}

		bool bc_scene_graph::update_actor(bc_actor& p_actor, const physics::bc_bound_box& p_previous_box)
		{
			return m_graph_node->update_actor(p_actor, p_previous_box);
		}

		bool bc_scene_graph::remove_actor(bc_actor& p_actor)
		{
			return m_graph_node->remove_actor(p_actor);
		}

		bc_scene_graph_buffer bc_scene_graph::get_actors(const bc_camera_frustum& p_camera_frustum) const
		{
			bc_scene_graph_buffer l_result;
			m_graph_node->get_actors(p_camera_frustum, l_result);

			return l_result;
		}

		void bc_scene_graph::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
		}

		void bc_scene_graph::render_debug_shapes(bc_shape_drawer& p_shape_drawer) const
		{
			m_graph_node->render_bound_boxes(p_shape_drawer);
		}

		void bc_scene_graph::clear()
		{
			bcAssert(m_graph_node);

			for (auto& l_actor : *m_graph_node)
			{
				l_actor.destroy();
			}

			m_graph_node->clear();
		}
	}
}