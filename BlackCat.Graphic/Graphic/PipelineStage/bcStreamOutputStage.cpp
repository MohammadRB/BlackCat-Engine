// [01/14/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include "Graphic/PipelineStage/bcStreamOutputStage.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		bc_stream_output_stage_state::bc_stream_output_stage_state()
			: m_stream_buffers(nullptr),
			m_stream_offsets(0)
		{
		}

		bc_stream_output_stage_state::~bc_stream_output_stage_state()
		{
		}

		void bc_stream_output_stage_state::set_to_initial_state()
		{
			m_stream_buffers.set_to_initial_state();
			m_stream_offsets.set_to_initial_state();
		}

		void bc_stream_output_stage_state::reset_tracking()
		{
			m_stream_buffers.reset_tracking();
			m_stream_offsets.reset_tracking();
		}
	}
}