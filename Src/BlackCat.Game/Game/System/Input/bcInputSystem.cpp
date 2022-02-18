// [05/16/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Platform/bcEvent.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_input_system::bc_input_system()
			: m_key_device(platform::bc_human_interface_device::create_key_device(0)),
			m_pointing_device(platform::bc_human_interface_device::create_pointing_device(0)),
			m_editor_camera(nullptr),
			m_editor_mode(false),
			m_window_has_focus(true)
		{
			auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();
			m_window_resize_event_handle = l_event_manager->register_event_listener<platform::bc_app_event_window_resize>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
			m_window_focus_event_handle = l_event_manager->register_event_listener<platform::bc_app_event_window_focus>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
			m_editor_mode_event_handle = l_event_manager->register_event_listener<bc_event_editor_mode>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
		}

		bc_input_system::bc_input_system(bc_input_system&& p_other) noexcept
			: m_window_resize_event_handle(std::move(p_other.m_window_resize_event_handle)),
			m_window_focus_event_handle(std::move(p_other.m_window_focus_event_handle)),
			m_editor_mode_event_handle(std::move(p_other.m_editor_mode_event_handle)),
			m_key_device(p_other.m_key_device),
			m_pointing_device(p_other.m_pointing_device),
			m_cameras(std::move(p_other.m_cameras)),
			m_editor_camera(p_other.m_editor_camera),
			m_editor_mode(p_other.m_editor_mode),
			m_window_has_focus(p_other.m_window_has_focus)
		{
			m_window_resize_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_window_focus_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_editor_mode_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
		}

		bc_input_system::~bc_input_system() = default;

		bc_input_system& bc_input_system::operator=(bc_input_system&& p_other) noexcept
		{
			m_window_resize_event_handle = std::move(p_other.m_window_resize_event_handle);
			m_window_focus_event_handle = std::move(p_other.m_window_focus_event_handle);
			m_editor_mode_event_handle = std::move(p_other.m_editor_mode_event_handle);
			m_key_device = p_other.m_key_device;
			m_pointing_device = p_other.m_pointing_device;
			m_cameras = std::move(p_other.m_cameras);
			m_editor_camera = p_other.m_editor_camera;
			m_editor_mode = p_other.m_editor_mode;
			m_window_has_focus = p_other.m_window_has_focus;

			m_window_resize_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_window_focus_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_editor_mode_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			
			return *this;
		}

		bci_camera* bc_input_system::add_camera(core::bc_unique_ptr<bci_camera> p_camera)
		{
			{
				core::bc_mutex_test_guard l_lock(m_cameras_mutex);
				
				m_cameras.push_back(std::move(p_camera));
				
				return m_cameras.back().get();
			}
		}

		bci_camera* bc_input_system::add_editor_camera(core::bc_unique_ptr<bci_camera> p_camera)
		{
			{
				core::bc_mutex_test_guard l_lock(m_cameras_mutex);

				m_cameras.push_back(std::move(p_camera));
				m_editor_camera = m_cameras.back().get();
				
				return m_cameras.back().get();
			}
		}

		void bc_input_system::remove_camera(const bci_camera* p_camera)
		{
			{
				core::bc_mutex_test_guard l_lock(m_cameras_mutex);

				const auto l_ite = std::find_if
				(
					std::begin(m_cameras),
					std::end(m_cameras),
					[p_camera](const core::bc_unique_ptr<bci_camera>& p_camera_entry)
					{
						return p_camera_entry.get() == p_camera;
					}
				);

				BC_ASSERT(l_ite != std::end(m_cameras));

				m_cameras.erase(l_ite);
			}
		}

		void bc_input_system::update(const core_platform::bc_clock::update_param& p_clock)
		{
			if(m_window_has_focus)
			{
				m_key_device.update();
				m_pointing_device.update();
			}

			if (auto* l_camera = get_camera())
			{
				l_camera->update(p_clock);
			}
		}

		void bc_input_system::_event_handler(core::bci_event& p_event)
		{
			auto* l_window_resize_event = core::bci_message::as<platform::bc_app_event_window_resize>(p_event);
			if (l_window_resize_event)
			{
				for(const auto& l_camera : m_cameras)
				{
					l_camera->set_projection(l_window_resize_event->width(), l_window_resize_event->height());
				}

				return;
			}

			auto* l_window_focus_event = core::bci_message::as<platform::bc_app_event_window_focus>(p_event);
			if (l_window_focus_event)
			{
				// TODO check if event belongs to main window
				m_window_has_focus = l_window_focus_event->get_focus();
				
				return;
			}

			auto* l_editor_mode_event = core::bci_message::as<bc_event_editor_mode>(p_event);
			if(l_editor_mode_event)
			{
				m_editor_mode = l_editor_mode_event->get_editor_mode();
			}

			return;
		}
	}
}