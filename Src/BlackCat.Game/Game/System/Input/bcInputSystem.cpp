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
		bc_input_system::bc_input_system(const bci_render_application_output_window* p_output_window)
			: m_event_manager(core::bc_get_service<core::bc_event_manager>()),
			m_output_window(p_output_window),
			m_key_device(platform::bc_human_interface_device::create_key_device(0)),
			m_pointing_device(platform::bc_human_interface_device::create_pointing_device(0)),
			m_editor_camera(nullptr),
			m_editor_mode(false),
			m_window_has_focus(true),
			m_pointing_visible(true)
		{
			m_window_resize_event_handle = m_event_manager->register_event_listener<platform::bc_app_event_window_resize>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
			m_window_focus_event_handle = m_event_manager->register_event_listener<platform::bc_app_event_window_focus>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
			m_editor_mode_event_handle = m_event_manager->register_event_listener<bc_event_editor_mode>
			(
				core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
			);
		}

		bc_input_system::bc_input_system(bc_input_system&& p_other) noexcept
			: m_event_manager(p_other.m_event_manager),
			m_output_window(p_other.m_output_window),
			m_key_device(p_other.m_key_device),
			m_pointing_device(p_other.m_pointing_device),
			m_cameras(std::move(p_other.m_cameras)),
			m_editor_camera(p_other.m_editor_camera),
			m_editor_mode(p_other.m_editor_mode),
			m_window_has_focus(p_other.m_window_has_focus),
			m_pointing_visible(p_other.m_pointing_visible),
			m_window_resize_event_handle(std::move(p_other.m_window_resize_event_handle)),
			m_window_focus_event_handle(std::move(p_other.m_window_focus_event_handle)),
			m_editor_mode_event_handle(std::move(p_other.m_editor_mode_event_handle))
		{
			m_window_resize_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_window_focus_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
			m_editor_mode_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler));
		}

		bc_input_system::~bc_input_system() = default;

		bc_input_system& bc_input_system::operator=(bc_input_system&& p_other) noexcept
		{
			m_event_manager = p_other.m_event_manager;
			m_output_window = p_other.m_output_window;
			m_key_device = p_other.m_key_device;
			m_pointing_device = p_other.m_pointing_device;
			m_cameras = std::move(p_other.m_cameras);
			m_editor_camera = p_other.m_editor_camera;
			m_editor_mode = p_other.m_editor_mode;
			m_window_has_focus = p_other.m_window_has_focus;
			m_pointing_visible = p_other.m_pointing_visible;
			m_window_resize_event_handle = std::move(p_other.m_window_resize_event_handle);
			m_window_focus_event_handle = std::move(p_other.m_window_focus_event_handle);
			m_editor_mode_event_handle = std::move(p_other.m_editor_mode_event_handle);

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

		void bc_input_system::set_pointing_device_visibility(bool p_show) noexcept
		{
			m_pointing_visible = p_show;
			m_pointing_device.set_visibility(p_show);
		}

		void bc_input_system::update(const platform::bc_clock::update_param& p_clock)
		{
			if(m_window_has_focus)
			{
				const auto l_key_states = m_key_device.update();
				auto l_pointing_state = m_pointing_device.update();

				if (m_output_window && !m_pointing_visible)
				{
					const auto l_center_x = m_output_window->get_left() + m_output_window->get_width() / 2;
					const auto l_center_y = m_output_window->get_top() + m_output_window->get_height() / 2;
					m_pointing_device.set_position(l_center_x, l_center_y);

					l_pointing_state.m_dx = l_pointing_state.m_x - l_center_x;
					l_pointing_state.m_dy = l_pointing_state.m_y - l_center_y;
					l_pointing_state.m_x = l_center_x;
					l_pointing_state.m_y = l_center_y;
				}

				platform::bc_app_event_pointing l_event_pointing(l_pointing_state);
				m_event_manager->process_event(l_event_pointing);

				for(const auto& l_key_state : l_key_states)
				{
					platform::bc_app_event_key l_key_event(std::get<0>(l_key_state), std::get<1>(l_key_state));
					m_event_manager->process_event(l_key_event);
				}
			}

			if (auto* l_camera = get_camera())
			{
				l_camera->update(p_clock);
			}
		}

		void bc_input_system::_event_handler(core::bci_event& p_event)
		{
			if (const auto* l_window_resize_event = core::bci_message::as<platform::bc_app_event_window_resize>(p_event))
			{
				for(const auto& l_camera : m_cameras)
				{
					l_camera->set_projection(l_window_resize_event->width(), l_window_resize_event->height());
				}
				return;
			}

			if (const auto* l_window_focus_event = core::bci_message::as<platform::bc_app_event_window_focus>(p_event))
			{
				const auto l_output_window_id = m_output_window ? m_output_window->get_id() : platform::bc_window::invalid_id;
				if(l_window_focus_event->get_window_id() != l_output_window_id)
				{
					return;
				}
				
				m_window_has_focus = l_window_focus_event->get_focus();

				if (!m_window_has_focus && !m_pointing_visible)
				{
					m_pointing_device.set_visibility(true);
				}
				else if (m_window_has_focus && !m_pointing_visible)
				{
					m_pointing_device.set_visibility(false);
				}
				return;
			}

			if (const auto* l_editor_mode_event = core::bci_message::as<bc_event_editor_mode>(p_event))
			{
				m_editor_mode = l_editor_mode_event->get_editor_mode();
				return;
			}
		}
	}
}