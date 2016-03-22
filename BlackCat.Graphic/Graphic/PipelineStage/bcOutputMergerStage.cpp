// [01/13/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include "Graphic/PipelineStage/bcOutputMergerStage.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		bc_output_merger_stage_state::bc_output_merger_stage_state()
			: //m_blend_state(nullptr),
			//m_depth_stencil_state(nullptr),
			m_blend_factors(bc_vector4f(0,0,0,0)),
			m_stencil_ref(0), 
			m_render_target_views(nullptr), 
			m_depth_target_view(nullptr), 
			m_unordered_access_views(nullptr)
			/*m_uav_initial_counts(0, bc_render_api_info::number_of_ps_cs_uav_registers())*/
		{
		}

		bc_output_merger_stage_state::~bc_output_merger_stage_state()
		{
		}

		void bc_output_merger_stage_state::set_to_initial_state()
		{
			//m_blend_state.set_to_initial_state();
			//m_depth_stencil_state.set_to_initial_state();
			m_stencil_ref.set_to_initial_state();
			m_render_target_views.set_to_initial_state();
			m_depth_target_view.set_to_initial_state();
			m_unordered_access_views.set_to_initial_state();
		}

		void bc_output_merger_stage_state::reset_tracking()
		{
			//m_blend_state.reset_tracking();
			//m_depth_stencil_state.reset_tracking();
			m_stencil_ref.reset_tracking();
			m_render_target_views.reset_tracking();
			m_depth_target_view.reset_tracking();
			m_unordered_access_views.reset_tracking();
		}

		bcUINT bc_output_merger_stage_state::associated_render_target_count() const
		{
			bcUINT l_count = 0;
			bcUINT l_render_target_slot_count = bc_render_api_info::number_of_om_render_target_slots();

			for (bcUINT i = 0; i < l_render_target_slot_count; ++i)
			{
				if (m_render_target_views.get(i) != nullptr)
					++l_count;
			}

			return l_count;
		}
	}
}