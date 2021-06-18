// [06/15/2021 MRB]

#include "BoX.Server/Application/bxApplication.h"

using namespace black_cat;

namespace box
{
	void bx_application::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
	}

	void bx_application::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
	}

	void bx_application::application_load_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_application::application_update(core_platform::bc_clock::update_param p_clock, bool p_is_partial_update)
	{
	}

	void bx_application::application_render(core_platform::bc_clock::update_param p_clock)
	{
	}

	bool bx_application::application_event(core::bci_event& p_event)
	{
		return false;
	}

	void bx_application::application_unload_content(core::bc_content_stream_manager* p_stream_manager)
	{
	}

	void bx_application::application_destroy()
	{
	}

	void bx_application::application_close_engine_components()
	{
	}
}