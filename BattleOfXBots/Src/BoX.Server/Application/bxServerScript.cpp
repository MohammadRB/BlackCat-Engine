// [06/20/2021 MRB]

#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Message/bcStringNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneChangeNetworkMessage.h"
#include "Game/Object/Scene/bcScene.h"
#include "BoX.Server/Application/bxServerScript.h"

namespace box
{
	bx_server_script::bx_server_script(game::bc_game_system& p_game_system, 
		game::bci_network_server_manager_hook& p_network_server_hook, 
		game::bci_network_message_visitor& p_message_visitor)
		: m_game_system(&p_game_system),
		m_network_server_hook(&p_network_server_hook),
		m_message_visitor(&p_message_visitor)
	{
	}

	bx_server_script::bx_server_script(const bx_server_script&) noexcept = default;

	bx_server_script::~bx_server_script() = default;

	bx_server_script& bx_server_script::operator=(const bx_server_script&) noexcept = default;

	platform::bc_script_variable bx_server_script::start(const platform::bc_script_variable& p_port)
	{
		if(!p_port.is_number())
		{
			return platform::bc_script_variable();
		}

		m_game_system->get_network_system().start_server(*m_network_server_hook, *m_message_visitor, p_port.as_integer());
		
		return platform::bc_script_variable();
	}

	platform::bc_script_variable bx_server_script::load_scene(const platform::bc_script_variable& p_scene_name)
	{
		if (!p_scene_name.is_string())
		{
			return platform::bc_script_variable();
		}

		const auto l_scene_name = p_scene_name.as_string().data();
		
		try
		{
			auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
			auto& l_file_system = m_game_system->get_file_system();
			
			auto l_scene = l_content_manager->load<game::bc_scene>
			(
				l_file_system.get_content_scene_path(l_scene_name.c_str()).c_str(),
				nullptr,
				core::bc_content_loader_parameter()
			);
			
			m_game_system->set_scene(std::move(l_scene));
		}
		catch (const std::exception& p_exception)
		{
			core::bc_log(core::bc_log_type::error) << "Error on loading scene '" << l_scene_name << "'. " << p_exception.what() << core::bc_lend;
		}

		return platform::bc_script_variable();
	}

	platform::bc_script_variable bx_server_script::send(const platform::bc_script_variable& p_str_content)
	{
		if(!p_str_content.is_string())
		{
			return platform::bc_script_variable();
		}

		auto l_str = core::bc_to_string(p_str_content.as_string().data());
		m_game_system->get_network_system().send_message(game::bc_string_network_message(std::move(l_str)));
		
		return platform::bc_script_variable();
	}
}