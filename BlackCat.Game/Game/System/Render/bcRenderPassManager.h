// [03/13/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderPass.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_render_pass_manager : public core_platform::bc_no_copy
		{
		public:
			bc_render_pass_manager();

			~bc_render_pass_manager();

			bc_render_pass_manager(bc_render_pass_manager&&) = default;

			bc_render_pass_manager& operator=(bc_render_pass_manager&&) = default;

			bc_irender_pass* get_pass(bcUINT p_location);

			bc_irender_pass* get_pass(core::bc_string p_name);

			void add_pass(bcUINT p_location, core::bc_unique_ptr< bc_irender_pass >&& p_pass);

			bool remove_pass(bcUINT p_location);

			bool remove_pass(core::bc_string p_name);

			void pass_initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device);

			void pass_update(core_platform::bc_clock::update_param p_clock_update_param);

			void pass_initialize_frame(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer);

			void pass_execute(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer);

			void before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters);

			void after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters);

			void pass_destroy(graphic::bc_device* p_device);

		protected:

		private:
			core::bc_vector< core::bc_unique_ptr< bc_irender_pass > > m_passes;
		};
	}
}
