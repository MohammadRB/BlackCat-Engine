// [14/09/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Game/Object/Scene/Component/bcPickupProxyComponent.h"
#include "Game/Object/Mesh/bcHeightMap.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_height_map_component : public bc_render_component, public bc_decal_resolver_component, public bc_pickup_proxy_component
	{
		BC_COMPONENT(hgt_map, true, false)

	public:
		bc_height_map_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_height_map_component(bc_height_map_component&&) noexcept;

		~bc_height_map_component() override;

		bc_height_map_component& operator=(bc_height_map_component&&) noexcept;

		core::bc_vector3f get_world_position() const noexcept;

		const bc_height_map& get_height_map() const noexcept;

		bc_height_map_ptr get_height_map_ptr() const noexcept;
		
		void initialize(const bc_actor_component_initialize_context& p_context) override;

		void load_instance(const bc_actor_component_load_context& p_context) override;
			
		void write_instance(const bc_actor_component_write_context& p_context) override;

		void handle_event(const bc_actor_component_event_context& p_context) override;
			
		void render(const bc_actor_component_render_context& p_context) const override;

		void add_decal(const bcCHAR* p_decal_name, 
		               const core::bc_vector3f& p_world_position, 
		               const core::bc_vector3f& p_dir, 
		               bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index) override;

		bc_pickup_proxy_result ray_pickup(const physics::bc_ray& p_ray) const override;

	private:
		bc_height_map_ptr m_height_map;
		core::bc_matrix4f m_transform;
	};

	inline core::bc_vector3f bc_height_map_component::get_world_position() const noexcept
	{
		return m_transform.get_translation();
	}

	inline const bc_height_map& bc_height_map_component::get_height_map() const noexcept
	{
		return *m_height_map;
	}

	inline bc_height_map_ptr bc_height_map_component::get_height_map_ptr() const noexcept
	{
		return m_height_map;
	}
}
