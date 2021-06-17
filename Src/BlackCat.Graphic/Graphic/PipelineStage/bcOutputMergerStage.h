// [01/13/2016 MRB]

#pragma once

#include "Core/Math/bcVector4f.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/PipelineStage/bcPixelStage.h"
#include "Graphic/Resource/View/bcResourceView.h"
#include "Graphic/Resource/View/bcRenderTargetView.h"
#include "Graphic/Resource/View/bcDepthStencilView.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_render_api >;

		class bc_output_merger_stage_state
		{
		public:
			bc_output_merger_stage_state() noexcept;

			bc_output_merger_stage_state(const bc_output_merger_stage_state&) noexcept = default;

			bc_output_merger_stage_state(bc_output_merger_stage_state&&) noexcept = default;

			~bc_output_merger_stage_state();

			bc_output_merger_stage_state& operator=(const bc_output_merger_stage_state&) noexcept = default;

			bc_output_merger_stage_state& operator=(bc_output_merger_stage_state&&) noexcept = default;

			void set_to_initial_state() noexcept;

			void reset_tracking() noexcept;

			bcUINT associated_render_target_count() const noexcept;

			//bc_pipeline_state_variable< bc_blend_state* > m_blend_state;
			//bc_pipeline_state_variable< bc_depth_stencil_state* > m_depth_stencil_state;
			bc_pipeline_state_variable< core::bc_vector4f > m_blend_factors;
			bc_pipeline_state_variable< bcUINT > m_stencil_ref;
			bc_pipeline_state_array_variable< bc_render_target_view, bc_render_api_info::number_of_om_render_target_slots() > m_render_target_views;
			bc_pipeline_state_variable< bc_depth_stencil_view > m_depth_target_view;
			//bc_pipeline_state_array_variable< bcUINT > m_uav_initial_counts;
		};

		inline bc_output_merger_stage_state::bc_output_merger_stage_state() noexcept
			: //m_blend_state(nullptr),
			  //m_depth_stencil_state(nullptr),
			m_blend_factors(core::bc_vector4f(0, 0, 0, 0)),
			m_stencil_ref(0),
			m_render_target_views(bc_render_target_view()),
			m_depth_target_view(bc_depth_stencil_view())
			/*m_uav_initial_counts(0, bc_render_api_info::number_of_ps_cs_uav_registers())*/
		{
		}

		inline bc_output_merger_stage_state::~bc_output_merger_stage_state()
		{
		}

		inline void bc_output_merger_stage_state::set_to_initial_state() noexcept
		{
			//m_blend_state.set_to_initial_state();
			//m_depth_stencil_state.set_to_initial_state();
			m_stencil_ref.set_to_initial_state();
			m_render_target_views.set_to_initial_state();
			m_depth_target_view.set_to_initial_state();
		}

		inline void bc_output_merger_stage_state::reset_tracking() noexcept
		{
			//m_blend_state.reset_tracking();
			//m_depth_stencil_state.reset_tracking();
			m_stencil_ref.reset_tracking();
			m_render_target_views.reset_tracking();
			m_depth_target_view.reset_tracking();
		}

		inline bcUINT bc_output_merger_stage_state::associated_render_target_count() const noexcept
		{
			bcUINT l_count = 0;
			constexpr bcUINT l_render_target_slot_count = bc_render_api_info::number_of_om_render_target_slots();

			for (bcUINT i = 0; i < l_render_target_slot_count; ++i)
			{
				if (m_render_target_views.get(i).is_valid())
				{
					++l_count;
				}
			}

			return l_count;
		}

		template<bc_render_api TRenderApi>
		struct bc_platform_output_merger_stage_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_output_merger_stage : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_output_merger_stage_pack<TRenderApi>;

		public:
			bc_platform_output_merger_stage(platform_pack p_pack);

			bc_platform_output_merger_stage(bc_platform_output_merger_stage&&) noexcept;

			~bc_platform_output_merger_stage();

			bc_platform_output_merger_stage& operator=(bc_platform_output_merger_stage&&) noexcept;

			void apply_required_state(bc_device_pipeline& p_pipeline, bc_pixel_stage& p_pixel_stage);

			void set_to_default_state(bc_device_pipeline& p_pipeline);

			bc_output_merger_stage_state& get_required_state()
			{
				return m_required_state;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;

			bc_output_merger_stage_state m_required_state;
		};

		using bc_output_merger_stage = bc_platform_output_merger_stage<g_current_render_api>;
	}
}