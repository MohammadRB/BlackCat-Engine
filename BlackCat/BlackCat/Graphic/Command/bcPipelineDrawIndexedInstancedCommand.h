// [02/26/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcGraphicDefinition.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "BlackCat/Graphic/Command/bcPipelineDrawIndexedCommand.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_draw_indexed_instanced_command_parameters
		{
		public:
			bc_draw_indexed_instanced_command_parameters(bcUINT p_index_count_per_instance,
				bcUINT p_instance_count,
				bcUINT p_start_index_location,
				bcINT p_base_vertex_location,
				bcUINT p_start_instance_location)
				: m_index_count_per_instance(p_index_count_per_instance),
				m_instance_count(p_instance_count),
				m_start_index_location(p_start_index_location),
				m_base_vertex_location(p_base_vertex_location),
				m_start_instance_location(p_start_instance_location)
			{
			}

			bcUINT m_index_count_per_instance;
			bcUINT m_instance_count;
			bcUINT m_start_index_location;
			bcINT m_base_vertex_location;
			bcUINT m_start_instance_location;
		};
		
		class bc_pipeline_draw_indexed_instanced_command : public bc_pipeline_draw_indexed_command
		{
		public:
			bc_pipeline_draw_indexed_instanced_command(bc_primitive p_primitive,
				bc_format p_index_format,
				bc_buffer_ptr p_vertex_buffer,
				bc_buffer_ptr p_index_buffer,
				bc_buffer_ptr p_instance_buffer);

			virtual ~bc_pipeline_draw_indexed_instanced_command() = default;

			void execute(bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) override;

		protected:
			bc_buffer_ptr m_instance_buffer;

		private:
		};
	}
}
