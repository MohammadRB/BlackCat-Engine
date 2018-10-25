// [06/13/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderState.h"
#include "Game/System/Render/bcRenderSystemParameter.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_render_thread;
		class bc_render_pass_state;

		class BC_GAME_DLL _bc_render_pass_state_handle_deleter
		{
		public:
			using handle_t = _bc_render_state_handle;

		public:
			_bc_render_pass_state_handle_deleter();

			_bc_render_pass_state_handle_deleter(bc_render_system* p_render_system);

			bc_render_pass_state* operator()(handle_t p_handle) const;

			void operator ()(bc_render_pass_state* p_render_state) const;

		protected:

		private:
			bc_render_system* m_render_system;
		};

		// Non-mutable object that represent whole configuration that is needed for a render pass.
		// Shader parameters will be mapped to device registers as they appear in their array
		class BC_GAME_DLL bc_render_pass_state
			: private core::bc_ref_count,
			core_platform::bc_no_copy
		{
		public:
			template< class T, class TDeleter >
			friend class core::bc_ref_count_handle;
			friend class bc_render_system;
			friend class bc_render_thread;

		public:
			bc_render_pass_state(bc_render_pass_state&&) noexcept = default;

			~bc_render_pass_state() = default;

			bc_render_pass_state& operator=(bc_render_pass_state&&) noexcept = default;

			const graphic::bc_device_pipeline_state& get_pipeline_state() const
			{
				return m_pipeline_state.get();
			}

			graphic::bc_viewport get_viewport() const
			{
				return m_viewport;
			}

			const bc_render_pass_state_render_target_view_array& get_render_targets() const
			{
				return m_shader_targets;
			}

			const graphic::bc_depth_stencil_view& get_depth_stencil() const
			{
				return m_shader_depth.get();
			}

			const bc_render_pass_state_sampler_array& get_samplers() const
			{
				return m_shader_samplers;
			}

			const bc_render_pass_state_resource_view_array& get_shader_views() const
			{
				return m_resource_views;
			}

			const bc_render_pass_state_constant_buffer_array& get_cbuffers() const
			{
				return m_shader_cbuffers;
			}

		protected:

		private:
			bc_render_pass_state(graphic::bc_device_pipeline_state p_pipeline_state,
				graphic::bc_viewport p_viewport,
				bc_render_pass_state_render_target_view_array&& p_shader_targets,
				graphic::bc_depth_stencil_view p_shader_depth,
				bc_render_pass_state_sampler_array&& p_shader_samplers,
				bc_render_pass_state_resource_view_array&& p_shader_views,
				bc_render_pass_state_constant_buffer_array&& p_shader_buffers);

			graphic::bc_device_pipeline_state_ptr m_pipeline_state;
			graphic::bc_viewport m_viewport;
			bc_render_pass_state_render_target_view_array m_shader_targets;
			graphic::bc_depth_stencil_view_ptr m_shader_depth;
			bc_render_pass_state_sampler_array m_shader_samplers;
			bc_render_pass_state_resource_view_array m_resource_views;
			bc_render_pass_state_constant_buffer_array m_shader_cbuffers;
		};

		using bc_render_pass_state_ptr = core::bc_ref_count_handle< bc_render_pass_state, _bc_render_pass_state_handle_deleter >;
	}
}