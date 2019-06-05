// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Event/bcEvent.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		/**
		* \brief Sent when an unexpected error occur in application
		*/
		class bc_app_event_error : public bc_app_event
		{
			BC_EVENT(app_error)

		public:
			explicit bc_app_event_error(const bcCHAR* p_message)
				: bc_app_event(event_name()),
				m_message(p_message)
			{
			}

			explicit bc_app_event_error(bc_string p_message)
				: bc_app_event(event_name()),
				m_message(std::move(p_message))
			{
			}

			bc_app_event_error(const bc_app_event_error&) = default;

			bc_app_event_error(bc_app_event_error&& p_other) noexcept(std::is_nothrow_move_constructible< bc_string >::value)
				: bc_app_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_app_event_error() = default;

			bc_app_event_error& operator =(const bc_app_event_error&) = default;

			bc_app_event_error& operator =(bc_app_event_error&& p_other) noexcept(std::is_nothrow_move_assignable< bc_string >::value)
			{
				m_message = std::move(p_other.m_message);

				return *this;
			}

			const bc_string& get_message() const noexcept(true)
			{
				return m_message;
			}

		protected:

		private:
			bc_string m_message;
		};

		class bc_app_event_debug : public bc_app_event
		{
			BC_EVENT(app_warning)

		public:
			explicit bc_app_event_debug(const bcCHAR* p_message)
				: bc_app_event(event_name()),
				m_message(p_message)
			{
			}

			explicit bc_app_event_debug(bc_string p_message)
				: bc_app_event(event_name()),
				m_message(std::move(p_message))
			{
			}

			bc_app_event_debug(const bc_app_event_debug&) = default;

			bc_app_event_debug(bc_app_event_debug&& p_other) noexcept(std::is_nothrow_move_constructible< bc_string >::value)
				: bc_app_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_app_event_debug() = default;

			bc_app_event_debug& operator =(const bc_app_event_debug&) = default;

			bc_app_event_debug& operator =(bc_app_event_debug&& p_other) noexcept(std::is_nothrow_move_assignable< bc_string >::value)
			{
				m_message = std::move(p_other.m_message);

				return *this;
			}

			const bc_string& get_message() const noexcept(true)
			{
				return m_message;
			}

		protected:

		private:
			bc_string m_message;
		};

		class bc_event_frame_update_start : public bc_event
		{
			BC_EVENT(frame_update_start)

		public:
			bc_event_frame_update_start()
				: bc_event(event_name())
			{
			}

			bc_event_frame_update_start(const bc_event_frame_update_start&) = default;

			~bc_event_frame_update_start() = default;

			bc_event_frame_update_start& operator=(const bc_event_frame_update_start&) = default;

		protected:

		private:
		};

		class bc_event_frame_update_finish : public bc_event
		{
			BC_EVENT(frame_update_finish)

		public:
			bc_event_frame_update_finish()
				: bc_event(event_name())
			{
			}

			bc_event_frame_update_finish(const bc_event_frame_update_finish&) = default;

			~bc_event_frame_update_finish() = default;

			bc_event_frame_update_finish& operator=(const bc_event_frame_update_finish&) = default;

		protected:

		private:
		};

		class bc_event_frame_render_start : public bc_event
		{
			BC_EVENT(frame_render_start)

		public:
			bc_event_frame_render_start()
				: bc_event(event_name())
			{
			}

			bc_event_frame_render_start(const bc_event_frame_render_start&) = default;

			~bc_event_frame_render_start() = default;

			bc_event_frame_render_start& operator=(const bc_event_frame_render_start&) = default;

		protected:

		private:
		};

		class bc_event_frame_render_finish : public bc_event
		{
			BC_EVENT(frame_render_finish)

		public:
			bc_event_frame_render_finish()
				: bc_event(event_name())
			{
			}

			bc_event_frame_render_finish(const bc_event_frame_render_finish&) = default;

			~bc_event_frame_render_finish() = default;

			bc_event_frame_render_finish& operator=(const bc_event_frame_render_finish&) = default;

		protected:

		private:
		};
	}
}