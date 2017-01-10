// [01/26/2016 MRB]

#pragma once

// Because explicit specialization of template classes must seen first by compiler
// in every 'imp' file we include all 'imp' files that are used
#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Shader/Parameter/bcConstantBufferParameter.h"
#include "GraphicImp/Shader/Parameter/bcSamplerParameter.h"
#include "GraphicImp/Shader/Parameter/bcResourceViewParameter.h"
#include "GraphicImp/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/PipelineStage/bcStreamOutputStage.h"
#include "GraphicImp/PipelineStage/bcRasterizerStage.h"
#include "GraphicImp/PipelineStage/bcOutputMergerStage.h"
#include "GraphicImp/PipelineStage/bcVertexStage.h"
#include "GraphicImp/PipelineStage/bcHullStage.h"
#include "GraphicImp/PipelineStage/bcDomainStage.h"
#include "GraphicImp/PipelineStage/bcGeometryStage.h"
#include "GraphicImp/PipelineStage/bcPixelStage.h"
#include "GraphicImp/PipelineStage/bcComputeStage.h"
#include "GraphicImp/Resource/bcResource.h"

#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/Device/bcDevicePipeline.h"
#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRefCountProxy.h"

namespace black_cat
{
	namespace graphic
	{
		class _bc_device_pipeline
		{
		public:
			_bc_device_pipeline()
				: m_device(nullptr),
				m_context(nullptr),
				m_query(nullptr),
				m_input_assembler_stage(bc_input_assembler_stage::platform_pack()),
				m_stream_output_stage(bc_stream_output_stage::platform_pack()),
				m_rasterizer_stage(bc_rasterizer_stage::platform_pack()),
				m_output_merger_stage(bc_output_merger_stage::platform_pack()),
				m_shader_stages
				{
					&m_vertex_shader_stage,
					&m_hull_shader_stage,
					&m_domain_shader_stage,
					&m_geometry_shader_stage,
					&m_pixel_shader_stage,
					&m_compute_shader_stage
				}
			{
			}

			bc_device* m_device;
			ID3D11DeviceContext* m_context;
			ID3D11Query* m_query;
			bc_input_assembler_stage m_input_assembler_stage;
			bc_stream_output_stage m_stream_output_stage;
			bc_rasterizer_stage m_rasterizer_stage;
			bc_output_merger_stage m_output_merger_stage;
			bc_vertex_stage	m_vertex_shader_stage;
			bc_hull_stage m_hull_shader_stage;
			bc_domain_stage	m_domain_shader_stage;
			bc_geometry_stage m_geometry_shader_stage;
			bc_pixel_stage m_pixel_shader_stage;
			bc_compute_stage m_compute_shader_stage;

			bc_programmable_stage* m_shader_stages[6];
		};

		using bc_device_pipeline_proxy = bc_refcount_proxy<_bc_device_pipeline>;

		template<>
		struct bc_platform_device_pipeline_pack< g_api_dx11 >
		{
			bc_device_pipeline_proxy* m_pipeline;
		};
	}
}