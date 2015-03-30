// [2/26/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"

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
				return "EventAppExit";
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
				return "EventWinResize";
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
				return "EventAppActivate";
			}

		protected:

		private:
			bool m_active;
		};
	}
}