// [2/26/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Platform/Application/bcWindow.h"
#include "Platform/Application/bcKeyDevice.h"
#include "Platform/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		class bc_app_event_exit : public core::bc_app_event
		{
		public:
			explicit bc_app_event_exit(bcINT32 p_exit_code)
				: bc_app_event(event_name()),
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

			static const bcCHAR* event_name()
			{
				return core::g_evt_app_exit;
			}

		protected:

		private:
			bcINT32 m_exit_code;
		};

		class bc_app_event_window_resize : public core::bc_app_event
		{
		public:
			bc_app_event_window_resize(bcUINT32 p_width, bcUINT32 p_height)
				: bc_app_event(event_name()),
				m_width(p_width),
				m_height(p_height)
			{
			}

			bc_app_event_window_resize(const bc_app_event_window_resize&) = default;

			~bc_app_event_window_resize() = default;

			bc_app_event_window_resize& operator =(const bc_app_event_window_resize&) = default;

			bcUINT32 width() const
			{
				return m_width;
			}

			bcUINT32 height() const
			{
				return m_height;
			}

			static const bcCHAR* event_name()
			{
				return core::g_evt_window_resize;
			}

		protected:

		private:
			bcUINT32 m_width, m_height;
		};

		class bc_app_event_active : public core::bc_app_event
		{
		public:
			explicit bc_app_event_active(bool active)
				: bc_app_event(event_name()),
				m_active(active)
			{
			}

			bc_app_event_active(const bc_app_event_active&) = default;

			~bc_app_event_active() = default;

			bc_app_event_active& operator =(const bc_app_event_active&) = default;

			bool activate() const
			{
				return m_active;
			}

			static const bcCHAR* event_name()
			{
				return core::g_evt_app_active;
			}

		protected:

		private:
			bool m_active;
		};

		class bc_app_event_window_close : public core::bc_app_event
		{
		public:
			explicit bc_app_event_window_close(bc_window_id p_window_id) noexcept
				: bc_app_event(event_name()),
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

			static const bcCHAR* event_name()
			{
				return core::g_evt_window_resize;
			}

		protected:

		private:
			bc_window_id m_window_id;
		};

		class bc_app_event_key : public core::bc_app_event
		{
		public:
			bc_app_event_key(bc_key p_key, bc_key_state p_state)
				: bc_app_event(event_name()),
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

			static const bcCHAR* event_name()
			{
				return core::g_evt_key;
			}

		protected:

		private:
			bc_key m_key;
			bc_key_state m_state;
		};

		class bc_app_event_pointing : public core::bc_app_event
		{
		public:
			explicit bc_app_event_pointing(bc_pointing_device_state p_state)
				: bc_app_event(event_name()),
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

			static const bcCHAR* event_name()
			{
				return core::g_evt_point;
			}

		protected:

		private:
			bc_pointing_device_state m_state;
		};
	}
}