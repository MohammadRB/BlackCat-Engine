// [02/12/2018 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_render_system;
	class bc_render_thread;
	class bc_render_state_buffer;
	class bc_sub_mesh;

	class BC_GAME_DLL bc_shape_drawer
	{
	public:
		bc_shape_drawer();

		bc_shape_drawer(bc_shape_drawer&& p_other) noexcept;

		~bc_shape_drawer();

		bc_shape_drawer& operator=(bc_shape_drawer&& p_other) noexcept;

		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_vertices
			 * \param p_indices
			 */
		void draw_point_list(core::bc_span<core::bc_vector3f> p_vertices, core::bc_span<bcUINT32> p_indices);
			
		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_box 
			 */
		void draw_wired_bound_box(const physics::bc_bound_box& p_box);

		/**
			 * \brief 
			 * \param p_box
			 * \ThreadSafe
			 * \param p_transform 
			 */
		void draw_wired_box(const physics::bc_shape_box& p_box, const physics::bc_transform& p_transform);

		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_sphere 
			 * \param p_transform 
			 */
		void draw_wired_sphere(const physics::bc_shape_sphere& p_sphere, const physics::bc_transform& p_transform);
			
		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_capsule 
			 * \param p_transform 
			 */
		void draw_wired_capsule(const physics::bc_shape_capsule& p_capsule, const physics::bc_transform& p_transform);

		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_camera_extend
			 */
		void draw_wired_frustum(const bci_camera::extend_points& p_camera_extend);

		/**
			 * \brief
			 * \ThreadSafe
			 * \param p_mesh
			 * \param p_world 
			 * \param p_mesh_transforms 
			 */
		void draw_wired_skeleton(const bc_sub_mesh& p_mesh, const core::bc_matrix4f& p_world, const bc_sub_mesh_mat4_transform& p_mesh_transforms);
			
		void render(bc_render_system& p_render_system, bc_render_thread& p_thread, bc_render_state_buffer& p_buffer);

		void clear_swap_buffers();

	private:
		void _update_buffers(bc_render_system& p_render_system, bc_render_thread& p_thread);
			
		void _destroy_buffers();
			
		platform::bc_mutex m_mutex;
		bcUINT32 m_buffer_write_index, m_buffer_read_index;
		core::bc_vector_movable<core::bc_vector3f> m_vertices[2];
		core::bc_vector_movable<bcUINT32> m_indices[2];

		graphic::bc_buffer_ref m_vb;
		graphic::bc_buffer_ref m_ib;
		bcUINT32 m_last_frame_indices_count;
		bc_render_state_ptr m_render_state;
	};
}
