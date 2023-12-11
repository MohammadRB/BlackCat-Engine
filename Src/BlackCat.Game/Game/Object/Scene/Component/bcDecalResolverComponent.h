// [28/02/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_decal_resolver_component : public bci_actor_abstract_component
	{
		BC_ABSTRACT_COMPONENT(dcl_rsv)
			
	public:
		virtual void add_decal(const bcCHAR* p_decal_name, 
		                       const core::bc_vector3f& p_world_position, 
		                       const core::bc_vector3f& p_world_direction, 
		                       bc_mesh_node::node_index_t p_attached_node_index = bc_mesh_node::s_invalid_index) = 0;
			
	protected:
		bc_decal_resolver_component() noexcept;

		bc_decal_resolver_component(bc_decal_resolver_component&&) noexcept;

		virtual ~bc_decal_resolver_component() override;

		bc_decal_resolver_component& operator=(bc_decal_resolver_component&&) noexcept;

		void load_instance(const bc_actor_component_load_context& p_context) override;
			
		void write_instance(const bc_actor_component_write_context& p_context) override;
	};

	inline bc_decal_resolver_component::bc_decal_resolver_component() noexcept = default;
	
	inline bc_decal_resolver_component::bc_decal_resolver_component(bc_decal_resolver_component&&) noexcept = default;

	inline bc_decal_resolver_component::~bc_decal_resolver_component() = default;

	inline bc_decal_resolver_component& bc_decal_resolver_component::operator=(bc_decal_resolver_component&&) noexcept = default;
}
