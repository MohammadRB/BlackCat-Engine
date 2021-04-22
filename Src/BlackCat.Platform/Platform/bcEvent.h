// [2/26/2015 MRB]

#pragma once

#include "Core/bcEvent.h"
#include "Platform/bcExport.h"
#include "Platform/Application/bcWindow.h"
#include "Platform/Application/bcKeyDevice.h"
#include "Platform/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		class bc_app_event_exit : public core::bc_app_event
		{
			BC_EVENT(app_ext)

		public:
			explicit bc_app_event_exit(bcINT32 p_exit_code)
				: bc_app_event(message_name()),
				m_exit_code(p_exit_code)
			{
			}

			bc_app_event_exit(const bc_app_event_exit&) = default;

			~bc_app_event_exit() = default;

			bc_app_event_exit& operator =(const bc_app_event_exit&) = default;

			bcINT32 exit_code() const
			{
				return m_exit_code;
			}

		private:
			bcINT32 m_exit_code;
		};

		class bc_app_event_active : public core::bc_app_event
		{
			BC_EVENT(app_acv)

		public:
			explicit bc_app_event_active(bool p_active)
				: bc_app_event(message_name()),
				m_active(p_active)
			{
			}

			bc_app_event_active(const bc_app_event_active&) = default;

			~bc_app_event_active() = default;

			bc_app_event_active& operator =(const bc_app_event_active&) = default;

			bool active() const
			{
				return m_active;
			}

		private:
			bool m_active;
		};

		class bc_app_event_window_focus : public core::bc_app_event
		{
			BC_EVENT(app_fcs)

		public:
			explicit bc_app_event_window_focus(bc_window_id p_window_id, bool p_focus)
				: bc_app_event(message_name()),
				m_window_id(p_window_id),
				p_focus(p_focus)
			{
			}

			bc_app_event_window_focus(const bc_app_event_window_focus&) = default;

			~bc_app_event_window_focus() = default;

			bc_app_event_window_focus& operator =(const bc_app_event_window_focus&) = default;

			bool get_focus() const
			{
				return p_focus;
			}

		private:
			bc_window_id m_window_id;
			bool p_focus;
		};

		/**
		* \brief Sent when window start to resizing or it's resizing loop end
		*/
		class bc_app_event_window_resize : public core::bc_app_event
		{
			BC_EVENT(app_rsz)

		public:
			bc_app_event_window_resize(bc_window_id p_window_id, bcUINT32 p_width, bcUINT32 p_height, bool p_started_to_resizing)
				: bc_app_event(message_name()),
				m_window_id(p_window_id),
				m_width(p_width),
				m_height(p_height),
				m_started_to_resizing(p_started_to_resizing)
			{
			}

			bc_app_event_window_resize(const bc_app_event_window_resize&) = default;

			~bc_app_event_window_resize() = default;

			bc_app_event_window_resize& operator =(const bc_app_event_window_resize&) = default;

			bc_window_id get_window_id() const noexcept
			{
				return m_window_id;
			}

			bcUINT32 width() const
			{
				return m_width;
			}

			bcUINT32 height() const
			{
				return m_height;
			}
			
			bool start_resizing() const noexcept
			{
				return m_started_to_resizing;
			}

			bool end_resizing() const noexcept
			{
				return !m_started_to_resizing;
			}
			
		private:
			bc_window_id m_window_id;
			bcUINT32 m_width, m_height;
			bool m_started_to_resizing;
		};

		/**
		 * \brief Sent when window get minimized, maximized or restored
		 */
		class bc_app_event_window_state : public core::bc_app_event
		{
			BC_EVENT(app_rzd)

		public:
			enum class state : bcBYTE
			{
				minimized,
				maximized,
				restored
			};

		public:
			bc_app_event_window_state(bc_window_id p_window_id, state p_state)
				: bc_app_event(message_name()),
				m_window_id(p_window_id),
				m_state(p_state)
			{
			}

			bc_app_event_window_state(const bc_app_event_window_state&) = default;

			~bc_app_event_window_state() = default;

			bc_app_event_window_state& operator =(const bc_app_event_window_state&) = default;

			bc_window_id get_window_id() const noexcept
			{
				return m_window_id;
			}

			state get_state() const noexcept
			{
				return m_state;
			}

		private:
			bc_window_id m_window_id;
			state m_state;
		};

		/**
		 * \brief Sent when a window get closed
		 */
		class bc_app_event_window_close : public core::bc_app_event
		{
			BC_EVENT(app_cls)

		public:
			explicit bc_app_event_window_close(bc_window_id p_window_id) noexcept
				: bc_app_event(message_name()),
				m_window_id(p_window_id)
			{
			}

			bc_app_event_window_close(const bc_app_event_window_close&) = default;

			~bc_app_event_window_close() = default;

			bc_app_event_window_close& operator =(const bc_app_event_window_close&) = default;

			bc_window_id get_window_id() const noexcept
			{
				return m_window_id;
			}

		private:
			bc_window_id m_window_id;
		};

		class bc_app_event_key : public core::bc_app_event
		{
			BC_EVENT(app_key)

		public:
			bc_app_event_key(bc_key p_key, bc_key_state p_state)
				: bc_app_event(message_name()),
				m_key(p_key),
				m_state(p_state)
			{
			}

			bc_app_event_key(const bc_app_event_key&) = default;

			~bc_app_event_key() = default;

			bc_app_event_key& operator =(const bc_app_event_key&) = default;

			bc_key get_key() const noexcept
			{
				return m_key;
			}

			bc_key_state get_key_state() const noexcept
			{
				return m_state;
			}

		private:
			bc_key m_key;
			bc_key_state m_state;
		};

		class bc_app_event_pointing : public core::bc_app_event
		{
			BC_EVENT(app_pnt)

		public:
			explicit bc_app_event_pointing(bc_pointing_device_state p_state)
				: bc_app_event(message_name()),
				m_state(p_state)
			{
			}

			bc_app_event_pointing(const bc_app_event_pointing&) = default;

			~bc_app_event_pointing() = default;

			bc_app_event_pointing& operator =(const bc_app_event_pointing&) = default;

			bc_pointing_device_state get_state() const noexcept
			{
				return m_state;
			}

		private:
			bc_pointing_device_state m_state;
		};
	}
}