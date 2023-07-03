// [13/01/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/Resource/State/bcViewPort.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline<g_current_render_api>;

		class bc_rasterizer_stage_state
		{
		public:
			bc_rasterizer_stage_state() noexcept;

			bc_rasterizer_stage_state(const bc_rasterizer_stage_state&) noexcept = default;

			bc_rasterizer_stage_state(bc_rasterizer_stage_state&&) noexcept = default;

			~bc_rasterizer_stage_state();

			bc_rasterizer_stage_state& operator=(const bc_rasterizer_stage_state&) noexcept = default;

			bc_rasterizer_stage_state& operator=(bc_rasterizer_stage_state&&) noexcept = default;

			void set_to_initial_state() noexcept;

			void reset_tracking() noexcept;

			//bc_pipeline_state_variable<bc_rasterizer_stage_state*> m_rasterizer_state;
			bc_pipeline_state_variable<bcUINT> m_viewport_count;
			bc_pipeline_state_array_variable<const bc_viewport*, bc_render_api_info::number_of_rs_viewport_scissorrect()> m_viewports;
			/*bc_pipeline_state_variable<int> m_scissor_rect_count;
			bc_pipeline_state_array_variable<D3D11_RECT> m_scissor_rects;*/
		};

		inline bc_rasterizer_stage_state::bc_rasterizer_stage_state() noexcept
			: //m_rasterizer_state(nullptr),
			m_viewport_count(0),
			m_viewports(nullptr)
		{
		}

		inline bc_rasterizer_stage_state::~bc_rasterizer_stage_state()
		{
		}

		inline void bc_rasterizer_stage_state::set_to_initial_state() noexcept
		{
			//m_rasterizer_state.set_to_initial_state();
			m_viewport_count.set_to_initial_state();
			m_viewports.set_to_initial_state();
		}

		inline void bc_rasterizer_stage_state::reset_tracking() noexcept
		{
			//m_rasterizer_state.reset_tracking();
			m_viewport_count.reset_tracking();
			m_viewports.reset_tracking();
		}

		template<bc_render_api TRenderApi>
		struct bc_platform_rasterizer_stage_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_rasterizer_stage : public platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_rasterizer_stage_pack<TRenderApi>;

		public:
			bc_platform_rasterizer_stage(platform_pack p_pack) noexcept;

			bc_platform_rasterizer_stage(bc_platform_rasterizer_stage&&) noexcept;

			~bc_platform_rasterizer_stage();

			bc_platform_rasterizer_stage& operator=(bc_platform_rasterizer_stage&&) noexcept;

			void apply_required_state(bc_device_pipeline& p_pipeline);

			void set_to_default_state(bc_device_pipeline& p_pipeline);

			bc_rasterizer_stage_state& get_required_state()
			{
				return m_required_state;
			}

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}
			
		private:
			platform_pack m_pack;
			bc_rasterizer_stage_state m_required_state;
		};

		using bc_rasterizer_stage = bc_platform_rasterizer_stage<g_current_render_api>;
	}
}