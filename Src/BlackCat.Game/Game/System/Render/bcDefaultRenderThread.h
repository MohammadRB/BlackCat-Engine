// [12/04/2020 MRB]

#pragma once

#include "Game/System/Render/bcRenderThread.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_default_render_thread : public bc_render_thread
		{
		public:
			using bc_render_thread::get_pipeline;
			using bc_render_thread::bind_render_pass_state;
			using bc_render_thread::unbind_render_pass_state;
			using bc_render_thread::bind_render_state;
			using bc_render_thread::unbind_render_state;
			using bc_render_thread::bind_compute_state;
			using bc_render_thread::unbind_compute_state;
			using bc_render_thread::bind_ps_constant_buffer_parameter;
			using bc_render_thread::unbind_ps_constant_buffer_parameter;
			using bc_render_thread::bind_ps_sampler_parameter;
			using bc_render_thread::unbind_ps_sampler_parameter;
			using bc_render_thread::bind_ps_shader_view_parameter;
			using bc_render_thread::unbind_ps_shader_view_parameter;
			using bc_render_thread::bind_om_blend_factors;
			using bc_render_thread::bind_om_stencil_ref;
			using bc_render_thread::pipeline_apply_states;
			using bc_render_thread::pipeline_set_default_states;
			using bc_render_thread::draw;
			using bc_render_thread::draw_instanced;
			using bc_render_thread::draw_instanced_indirect;
			using bc_render_thread::draw_indexed;
			using bc_render_thread::draw_indexed_instanced;
			using bc_render_thread::draw_indexed_instanced_indirect;
			using bc_render_thread::dispatch;
			using bc_render_thread::dispatch_indirect;
			using bc_render_thread::clear_buffers;
			using bc_render_thread::update_subresource;
			using bc_render_thread::copy_subresource;
			using bc_render_thread::copy_resource;
			using bc_render_thread::copy_structure_count;
			using bc_render_thread::resolve_subresource;
			
		public:
			bc_default_render_thread() noexcept = default;

			bc_default_render_thread(bc_default_render_thread&&) noexcept = default;

			~bc_default_render_thread() noexcept = default;

			bc_default_render_thread& operator=(bc_default_render_thread&&) noexcept = default;

			/**
			 * \brief Start capturing command lists and put pipeline to lock state
			 */
			void start() noexcept;

			/**
			 * \brief Stop capturing command lists and reset pipeline state and lock
			 */
			void finish() noexcept;

			void reset();

			void reset(graphic::bc_device_pipeline_ref p_pipeline);
		};
	}
}