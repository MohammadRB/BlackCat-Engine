// [17/03/2022 MRB]

#pragma once

#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "GraphicImp/Device/bcDeviceOutput.h"

namespace black_cat::core
{
	class bc_content_stream_manager;
}

namespace black_cat::game
{
	class bc_physics_system;

	struct bc_render_system_parameter
	{
		bc_render_system_parameter(core::bc_content_stream_manager& p_content_stream,
			bc_physics_system& p_physics_system,
			graphic::bc_device_output p_render_output,
			bcUINT32 p_device_backbuffer_width,
			bcUINT32 p_device_backbuffer_height,
			graphic::bc_format p_device_backbuffer_format,
			core::bc_string_view p_preferred_gpu)
			: m_content_stream(p_content_stream),
			  m_physics_system(p_physics_system),
			  m_render_output(std::move(p_render_output)),
			  m_device_backbuffer_width(p_device_backbuffer_width),
			  m_device_backbuffer_height(p_device_backbuffer_height),
			  m_device_backbuffer_format(p_device_backbuffer_format),
			  m_preferred_gpu(p_preferred_gpu)
		{
		}

		core::bc_content_stream_manager& m_content_stream;
		bc_physics_system& m_physics_system;
		graphic::bc_device_output m_render_output;
		bcUINT32 m_device_backbuffer_width;
		bcUINT32 m_device_backbuffer_height;
		graphic::bc_format m_device_backbuffer_format;
		core::bc_string_view m_preferred_gpu;
	};

	struct bc_game_system_parameter
	{
		bc_game_system_parameter(core::bc_content_stream_manager& p_content_stream,
			core::bc_query_manager& p_query_manager, 
			core::bc_event_manager& p_event_manager)
			: m_content_stream(p_content_stream),
			  m_query_manager(p_query_manager),
			  m_event_manager(p_event_manager),
			  m_output_window(nullptr),
			  m_device_backbuffer_width(0),
			  m_device_backbuffer_height(0),
			  m_device_backbuffer_format(graphic::bc_format::unknown)
		{
		}

		bc_game_system_parameter(core::bc_content_stream_manager& p_content_stream,
			core::bc_query_manager& p_query_manager,
			core::bc_event_manager& p_event_manager,
			const bci_render_application_output_window& p_output_window,
			graphic::bc_format p_back_buffer_format)
			: m_content_stream(p_content_stream),
			  m_query_manager(p_query_manager),
			  m_event_manager(p_event_manager),
			  m_output_window(&p_output_window),
			  m_device_backbuffer_width(p_output_window.get_width()),
			  m_device_backbuffer_height(p_output_window.get_height()),
			  m_device_backbuffer_format(p_back_buffer_format)
		{
		}

		core::bc_content_stream_manager& m_content_stream;
		core::bc_query_manager& m_query_manager;
		core::bc_event_manager& m_event_manager;
		const bci_render_application_output_window* m_output_window;
		bcUINT32 m_device_backbuffer_width;
		bcUINT32 m_device_backbuffer_height;
		graphic::bc_format m_device_backbuffer_format;
	};
}
