// [01/14/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include "Graphic/PipelineStage/bcRasterizerStage.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		bc_rasterizer_stage_state::bc_rasterizer_stage_state()
			: //m_rasterizer_state(nullptr),
			m_viewport_count(0),
			m_viewports(nullptr)
		{
		}

		bc_rasterizer_stage_state::~bc_rasterizer_stage_state()
		{
		}

		void bc_rasterizer_stage_state::set_to_initial_state()
		{
			//m_rasterizer_state.set_to_initial_state();
			m_viewport_count.set_to_initial_state();
			m_viewports.set_to_initial_state();
		}

		void bc_rasterizer_stage_state::reset_tracking()
		{
			//m_rasterizer_state.reset_tracking();
			m_viewport_count.reset_tracking();
			m_viewports.reset_tracking();
		}
	}
}