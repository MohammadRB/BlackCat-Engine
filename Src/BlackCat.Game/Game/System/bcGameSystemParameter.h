// [03/17/2022 MRB]

#pragma once

#include "Game/Application/bcIRenderApplicationOutputWindow.h"
#include "GraphicImp/Device/bcDeviceOutput.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}

	namespace game
	{
		class bc_physics_system;

		struct bc_render_system_parameter
		{
			bc_render_system_parameter(core::bc_content_stream_manager& p_content_stream,
				bc_physics_system& p_physics_system,
				bcUINT32 p_device_backbuffer_width,
				bcUINT32 p_device_backbuffer_height,
				graphic::bc_format p_device_backbuffer_format,
				graphic::bc_device_output p_render_output)
				: m_content_stream(p_content_stream),
				m_physics_system(p_physics_system),
				m_device_backbuffer_width(p_device_backbuffer_width),
				m_device_backbuffer_height(p_device_backbuffer_height),
				m_device_backbuffer_format(p_device_backbuffer_format),
				m_render_output(std::move(p_render_output))
			{
			}

			core::bc_content_stream_manager& m_content_stream;
			bc_physics_system& m_physics_system;
			bcUINT32 m_device_backbuffer_width;
			bcUINT32 m_device_backbuffer_height;
			graphic::bc_format m_device_backbuffer_format;
			graphic::bc_device_output m_render_output;
		};

		struct bc_game_system_parameter
		{
			bc_game_system_parameter(core::bc_content_stream_manager& p_content_stream,
				core::bc_query_manager& p_query_manager, 
				core::bc_event_manager& p_event_manager)
				: m_content_stream(p_content_stream),
				m_query_manager(p_query_manager),
				m_event_manager(p_event_manager),
				m_device_backbuffer_width(0),
				m_device_backbuffer_height(0),
				m_device_backbuffer_format(graphic::bc_format::unknown),
				m_output_window(nullptr)
			{
			}

			bc_game_system_parameter(core::bc_content_stream_manager& p_content_stream,
				core::bc_query_manager& p_query_manager,
				core::bc_event_manager& p_event_manager,
				const bci_render_application_output_window& p_render_window,
				graphic::bc_format p_back_buffer_format)
				: m_content_stream(p_content_stream),
				m_query_manager(p_query_manager),
				m_event_manager(p_event_manager),
				m_device_backbuffer_width(p_render_window.get_width()),
				m_device_backbuffer_height(p_render_window.get_height()),
				m_device_backbuffer_format(p_back_buffer_format),
				m_output_window(&p_render_window)
			{
			}

			core::bc_content_stream_manager& m_content_stream;
			core::bc_query_manager& m_query_manager;
			core::bc_event_manager& m_event_manager;
			bcUINT32 m_device_backbuffer_width;
			bcUINT32 m_device_backbuffer_height;
			graphic::bc_format m_device_backbuffer_format;
			const bci_render_application_output_window* m_output_window;
		};
	}
}
