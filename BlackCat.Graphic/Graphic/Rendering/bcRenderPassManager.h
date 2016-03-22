// [03/13/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcServiceManager.h"
#include "Graphic/Device/bcDevicePipeline.h"
#include "Graphic/Device/Command/bcDeviceCommandExecuter.h"
#include "Graphic/Rendering/bcRenderPass.h"

namespace black_cat
{
	namespace graphic
	{
		class BC_GRAPHIC_DLL_EXP bc_render_pass_manager : public core::bc_iservice
		{
		public:
			bc_render_pass_manager();

			~bc_render_pass_manager();

			bc_render_pass_manager(bc_render_pass_manager&&) = default;

			bc_render_pass_manager& operator=(bc_render_pass_manager&&) = default;

			void add_pass(bcUINT p_location, core::bc_unique_ptr< bc_irender_pass >&& p_pass);

			bool remove_pass(bcUINT p_location);

			bool remove_pass(core::bc_string p_name);

			void pass_initialize_resources(bc_device* p_device);

			void pass_update(bcFLOAT32 p_time_elapsed);

			void pass_initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline);

			void pass_execute(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline);

			void pass_destroy(bc_device* p_device);

			static core::bc_string service_name()
			{
				return "Render_Pass_Manager";
			}

		protected:

		private:
			core::bc_vector< core::bc_unique_ptr< bc_irender_pass > > m_passes;
		};
	}
}