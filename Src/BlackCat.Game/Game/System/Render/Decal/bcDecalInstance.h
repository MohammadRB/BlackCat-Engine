// [23/02/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Render/Decal/bcDecal.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_decal_manager;
	class bc_decal_manager_container;
	class bc_decal_instance;
		
	class BC_GAME_DLL bc_decal_instance_deleter
	{
	public:
		bc_decal_instance_deleter();

		explicit bc_decal_instance_deleter(bc_decal_manager_container& p_manager);

		void operator()(bc_decal_instance* p_ptr) const;

	private:
		bc_decal_manager_container* m_container;
	};
		
	class BC_GAME_DLL bc_decal_instance : public core::bc_ref_count
	{
	public:
		bc_decal_instance(bc_decal_ptr p_decal,
		                  bc_actor p_actor,
		                  const core::bc_vector3f& p_local_position,
		                  const core::bc_matrix3f& p_local_rotation,
		                  bc_mesh_node::node_index_t p_attached_node_index) noexcept;

		bc_decal_instance(bc_decal_ptr p_decal,
		                  const bc_actor& p_actor,
		                  const core::bc_vector3f& p_local_position,
		                  const core::bc_matrix3f& p_local_rotation,
		                  bc_actor_render_group p_render_group,
		                  bc_mesh_node::node_index_t p_attached_node_index) noexcept;

		bc_decal_instance(bc_decal_instance&&) noexcept;

		~bc_decal_instance();

		bc_decal_instance& operator=(bc_decal_instance&&) noexcept;

		bc_decal* get_decal() const noexcept;
			
		bc_decal_ptr get_decal_ptr() const noexcept;

		bc_actor get_actor() const noexcept;

		void set_local_position(const core::bc_vector3f& p_position) noexcept;
			
		const core::bc_vector3f& get_local_position() const noexcept;

		void set_local_rotation(const core::bc_matrix3f& p_rotation) noexcept;
			
		const core::bc_matrix3f& get_local_rotation() const noexcept;
			
		bc_mesh_node::node_index_t get_attached_node_index() const noexcept;

		bc_actor_render_group get_render_group() const noexcept;
			
		/**
			 * \brief Apply decal local transforms and scale then set decal world transform
			 * \param p_world_transform
			 */
		void set_world_transform(const core::bc_matrix4f& p_world_transform) noexcept;

		const core::bc_matrix4f& get_world_transform() const noexcept;

		std::pair<physics::bc_shape_box, physics::bc_transform> get_box() const noexcept;
		
	private:
		bc_decal_ptr m_decal;
		bc_actor m_actor;
		core::bc_vector3f m_local_position;
		core::bc_matrix3f m_local_rotation;
		bc_actor_render_group m_render_group;
		bc_mesh_node::node_index_t m_attached_node_index;
		core::bc_matrix4f m_world_transform;
	};

	using bc_decal_instance_ptr = core::bc_ref_count_ptr<bc_decal_instance, bc_decal_instance_deleter>;

	inline bc_decal_instance_deleter::bc_decal_instance_deleter()
		: m_container(nullptr)
	{
	}

	inline bc_decal_instance_deleter::bc_decal_instance_deleter(bc_decal_manager_container& p_manager)
		: m_container(&p_manager)
	{
	}

	inline bc_decal_instance::bc_decal_instance(bc_decal_ptr p_decal,
	                                            bc_actor p_actor,
	                                            const core::bc_vector3f& p_local_position,
	                                            const core::bc_matrix3f& p_local_rotation,
	                                            bc_mesh_node::node_index_t p_attached_node_index) noexcept
		: m_decal(std::move(p_decal)),
		  m_actor(std::move(p_actor)),
		  m_local_position(p_local_position),
		  m_local_rotation(p_local_rotation),
		  m_render_group(m_decal->get_group()),
		  m_attached_node_index(p_attached_node_index)
	{
	}

	/**
		 * \brief 
		 * \param p_decal 
		 * \param p_actor 
		 * \param p_local_position 
		 * \param p_local_rotation 
		 * \param p_render_group 
		 * \param p_attached_node_index 
		 */
	inline bc_decal_instance::bc_decal_instance(bc_decal_ptr p_decal,
	                                            const bc_actor& p_actor,
	                                            const core::bc_vector3f& p_local_position,
	                                            const core::bc_matrix3f& p_local_rotation,
	                                            bc_actor_render_group p_render_group,
	                                            bc_mesh_node::node_index_t p_attached_node_index) noexcept
		: m_decal(std::move(p_decal)),
		  m_actor(p_actor),
		  m_local_position(p_local_position),
		  m_local_rotation(p_local_rotation),
		  m_render_group(p_render_group),
		  m_attached_node_index(p_attached_node_index)
	{
	}
		
	inline bc_decal_instance::bc_decal_instance(bc_decal_instance&&) noexcept = default;

	inline bc_decal_instance::~bc_decal_instance() = default;

	inline bc_decal_instance& bc_decal_instance::operator=(bc_decal_instance&&) noexcept = default;

	inline bc_decal* bc_decal_instance::get_decal() const noexcept
	{
		return m_decal.get();
	}

	inline bc_decal_ptr bc_decal_instance::get_decal_ptr() const noexcept
	{
		return m_decal;
	}

	inline bc_actor bc_decal_instance::get_actor() const noexcept
	{
		return m_actor;
	}
				
	inline const core::bc_vector3f& bc_decal_instance::get_local_position() const noexcept
	{
		return m_local_position;
	}
		
	inline const core::bc_matrix3f& bc_decal_instance::get_local_rotation() const noexcept
	{
		return m_local_rotation;
	}
		
	inline bc_mesh_node::node_index_t bc_decal_instance::get_attached_node_index() const noexcept
	{
		return m_attached_node_index;
	}

	inline bc_actor_render_group bc_decal_instance::get_render_group() const noexcept
	{
		return m_render_group;
	}
		
	inline const core::bc_matrix4f& bc_decal_instance::get_world_transform() const noexcept
	{
		return m_world_transform;
	}
}
