// [14/07/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat::game
{
	bc_scene_graph::bc_scene_graph(core::bc_unique_ptr<bci_scene_graph_node> p_scene_graph) noexcept
		: m_graph_node(std::move(p_scene_graph))
	{
	}

	bc_scene_graph::~bc_scene_graph() noexcept
	{
		if(m_graph_node)
		{
			clear();
		}
	}

	physics::bc_bound_box bc_scene_graph::get_bound_box() const noexcept
	{
		return m_graph_node->get_bound_box();
	}

	bcSIZE bc_scene_graph::get_actors_count() const noexcept
	{
		return m_graph_node->get_actors_count();
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

	bool bc_scene_graph::add_actor(bc_actor& p_actor) noexcept
	{
		return m_graph_node->add_actor(p_actor);
	}

	bool bc_scene_graph::update_actor(bc_actor& p_actor) noexcept
	{
		return m_graph_node->update_actor(p_actor);
	}

	bool bc_scene_graph::remove_actor(bc_actor& p_actor) noexcept
	{
		return m_graph_node->remove_actor(p_actor);
	}

	bc_actor bc_scene_graph::get_actor(const physics::bc_ray& p_ray, bc_scene_graph_ray_query_filter_callback* p_filter) const noexcept
	{
		bc_scene_graph_ray_query_hit l_result;
		m_graph_node->get_actor(p_ray, l_result, p_filter);

		return l_result.m_actor;
	}

	bc_scene_graph_buffer bc_scene_graph::get_actors(const bc_camera_frustum& p_camera_frustum) const noexcept
	{
		bc_scene_graph_buffer l_result;
		m_graph_node->get_actors(p_camera_frustum, l_result);

		return l_result;
	}

	void bc_scene_graph::update(const platform::bc_clock::update_param& p_clock) noexcept
	{
		m_graph_node->update(p_clock);
	}

	void bc_scene_graph::draw_debug_shapes(bc_shape_drawer& p_shape_drawer) const
	{
		m_graph_node->draw_debug_shapes(p_shape_drawer);
	}

	void bc_scene_graph::clear() noexcept
	{
		const auto l_actors_count = m_graph_node->get_actors_count();
		auto l_actors_iterated = 0U;

		for (auto& l_actor : *m_graph_node)
		{
			l_actor.destroy();
			l_actors_iterated++;
		}

		BC_ASSERT(l_actors_iterated == l_actors_count);
		m_graph_node->clear();
	}

	bool bc_scene_graph::is_valid() const noexcept
	{
		return m_graph_node != nullptr;
	}
}
