// [12/02/2018 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;

		class BC_GAME_DLL bc_shape_drawer
		{
		public:
			bc_shape_drawer();

			bc_shape_drawer(bc_shape_drawer&& p_other) noexcept;

			~bc_shape_drawer();

			bc_shape_drawer& operator=(bc_shape_drawer&& p_other) noexcept;

			/**
			 * \brief Thread-Safe function
			 * \param p_box 
			 */
			void render_wired_box(const physics::bc_bound_box& p_box);

			/**
			 * \brief Thread-Safe function
			 * \param p_camera 
			 */
			void render_wired_frustum(const game::bc_icamera& p_camera);

			void update_buffers(bc_render_system& p_render_system, bc_render_thread& p_thread);

			void render(bc_render_system& p_render_system, bc_render_thread& p_thread);

			void clear_buffers();

			void destroy_buffers();

		protected:

		private:
			core_platform::bc_mutex m_mutex;
			core::bc_vector_movale<core::bc_vector3f> m_vertices;
			core::bc_vector_movale<bcUINT32> m_indices;

			graphic::bc_buffer_ptr m_vb;
			graphic::bc_buffer_ptr m_ib;
			bcUINT32 m_last_frame_indices_count;
			bc_render_state_ptr m_render_state;
		};
	}
}