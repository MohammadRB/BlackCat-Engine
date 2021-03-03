// [02/23/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Render/Decal/bcDecal.h"

namespace black_cat
{
	namespace game
	{
		class bc_decal_manager;
		class bc_decal_instance;
		
		class bc_decal_instance_deleter
		{
		public:
			bc_decal_instance_deleter();

			explicit bc_decal_instance_deleter(bc_decal_manager& p_manager);

			void operator()(bc_decal_instance* p_ptr) const;

		private:
			bc_decal_manager* m_manager;
		};
		
		class bc_decal_instance : public core::bc_ref_count
		{
		public:
			bc_decal_instance(bc_decal_ptr p_decal,
				const bc_actor& p_actor,
				const core::bc_vector3f& p_local_position,
				const core::bc_matrix3f& p_local_rotation,
				bc_mesh_node::node_index_t p_attached_node_index) noexcept;

			bc_decal_instance(bc_decal_instance&&) noexcept;

			~bc_decal_instance();

			bc_decal_instance& operator=(bc_decal_instance&&) noexcept;

			bc_decal* get_decal() const noexcept;
			
			bc_decal_ptr get_decal_ptr() const noexcept;

			bc_actor get_actor() const noexcept;
			
			const core::bc_vector3f& get_local_position() const noexcept;

			const core::bc_matrix3f& get_local_rotation() const noexcept;
			
			bc_mesh_node::node_index_t get_attached_node_index() const noexcept;

			/**
			 * \brief Apply decal local transforms and scale and set decal world transform
			 * \param p_transform
			 */
			void set_world_transform(const core::bc_matrix4f& p_transform) noexcept;

			const core::bc_matrix4f& get_world_transform() const noexcept;
		
		private:
			bc_decal_ptr m_decal;
			bc_actor m_actor;
			core::bc_vector3f m_local_position;
			core::bc_matrix3f m_local_rotation;
			bc_mesh_node::node_index_t m_attached_node_index;
			core::bc_matrix4f m_world_transform;
		};

		using bc_decal_instance_ptr = core::bc_ref_count_ptr<bc_decal_instance, bc_decal_instance_deleter>;

		inline bc_decal_instance_deleter::bc_decal_instance_deleter()
			: m_manager(nullptr)
		{
		}

		inline bc_decal_instance_deleter::bc_decal_instance_deleter(bc_decal_manager& p_manager)
			: m_manager(&p_manager)
		{
		}

		inline bc_decal_instance::bc_decal_instance(bc_decal_ptr p_decal,
			const bc_actor& p_actor,
			const core::bc_vector3f& p_local_position,
			const core::bc_matrix3f& p_local_rotation,
			bc_mesh_node::node_index_t p_attached_node_index) noexcept
			: m_decal(std::move(p_decal)),
			m_actor(p_actor),
			m_local_position(p_local_position),
			m_local_rotation(p_local_rotation),
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

		inline void bc_decal_instance::set_world_transform(const core::bc_matrix4f& p_transform) noexcept
		{
			core::bc_matrix4f l_decal_local_transform;
			l_decal_local_transform.make_identity();

			l_decal_local_transform.set_rotation(get_local_rotation());
			l_decal_local_transform.set_translation(get_local_position());

			const auto l_decal_width = m_decal->get_width();
			const auto l_decal_height = m_decal->get_height();
			const auto l_decal_depth = m_decal->get_depth();
			const auto l_scale = core::bc_matrix4f::scale_matrix_xyz(l_decal_width, l_decal_depth, l_decal_height);

			m_world_transform = l_scale * l_decal_local_transform * p_transform;
		}

		inline const core::bc_matrix4f& bc_decal_instance::get_world_transform() const noexcept
		{
			return m_world_transform;
		}
	}	
}