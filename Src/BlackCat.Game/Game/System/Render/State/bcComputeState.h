// [09/06/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "Game/bcExport.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/bcRenderSystemParameter.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_render_thread;
		class bc_compute_state;

		class BC_GAME_DLL _bc_compute_state_handle_deleter
		{
		public:
			_bc_compute_state_handle_deleter();

			_bc_compute_state_handle_deleter(bc_render_system* p_render_system);

			void operator ()(bc_compute_state* p_compute_state) const;

		private:
			bc_render_system* m_render_system;
		};

		/**
		 * \brief Shader parameters will be mapped to device registers as they appear in their array
		 */
		class BC_GAME_DLL bc_compute_state
			: public core::bc_ref_count,
			platform::bc_no_copy
		{
		public:
			template< class T, class TDeleter >
			friend class core::bc_ref_count_handle;
			friend class bc_render_system;
			friend class bc_render_thread;

		public:
			bc_compute_state(bc_compute_state&&) noexcept;

			~bc_compute_state();

			bc_compute_state& operator=(bc_compute_state&&) noexcept;
			
		private:
			bc_compute_state(graphic::bc_device_compute_state p_compute_state,
				bc_compute_state_sampler_array&& p_samplers,
				bc_compute_state_resource_view_array&& p_shader_views,
				bc_compute_state_unordered_view_array&& p_unordered_views,
				bc_compute_state_constant_buffer_array&& p_cbuffers);

			graphic::bc_device_compute_state m_compute_state;
			bc_compute_state_sampler_array m_samplers;
			bc_compute_state_resource_view_array m_resource_views;
			bc_compute_state_unordered_view_array m_unordered_views;
			bc_compute_state_constant_buffer_array m_cbuffers;
		};

		using bc_compute_state_ptr = core::bc_ref_count_ptr< bc_compute_state, _bc_compute_state_handle_deleter >;
	}
}