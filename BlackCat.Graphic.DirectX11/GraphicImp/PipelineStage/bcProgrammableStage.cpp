// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcProgrammableStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"

namespace black_cat
{
	namespace graphic
	{
		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_programmable_stage<g_api_dx11>::bc_platform_programmable_stage()
			: m_pack()
		{
		};

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_programmable_stage<g_api_dx11>::bc_platform_programmable_stage(bc_platform_programmable_stage&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		};

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_programmable_stage<g_api_dx11>::~bc_platform_programmable_stage()
		{
		};

		template<>
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_programmable_stage< g_api_dx11 >& bc_platform_programmable_stage< g_api_dx11 >::operator=(bc_platform_programmable_stage&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_programmable_stage<g_api_dx11>::apply_required_state(bc_device_pipeline* p_pipeline)
		{
			apply_shader_program(p_pipeline);
			apply_constant_buffers(p_pipeline);
			apply_sampler_states(p_pipeline);
			apply_shader_resource_views(p_pipeline);
			/*apply_unordered_access_views(p_pipeline);*/

			m_required_state.reset_tracking();
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_programmable_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline* p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}