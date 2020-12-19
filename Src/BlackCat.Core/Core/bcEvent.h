// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Messaging/Event/bcEvent.h"
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
			BC_EVENT(app_err)

		public:
			explicit bc_app_event_error(const bcCHAR* p_message)
				: bc_app_event(message_name()),
				m_message(p_message)
			{
			}

			explicit bc_app_event_error(bc_string p_message)
				: bc_app_event(message_name()),
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

			const bc_string& get_message() const noexcept
			{
				return m_message;
			}

		private:
			bc_string m_message;
		};

		class bc_app_event_debug : public bc_app_event
		{
			BC_EVENT(app_wrn)

		public:
			explicit bc_app_event_debug(const bcCHAR* p_message)
				: bc_app_event(message_name()),
				m_message(p_message)
			{
			}

			explicit bc_app_event_debug(bc_string p_message)
				: bc_app_event(message_name()),
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

			const bc_string& get_message() const noexcept
			{
				return m_message;
			}

		private:
			bc_string m_message;
		};

		class bc_event_frame_update_start : public bc_app_event
		{
			BC_EVENT(fr_up_s)

		public:
			bc_event_frame_update_start()
				: bc_app_event(message_name())
			{
			}

			bc_event_frame_update_start(const bc_event_frame_update_start&) = default;

			~bc_event_frame_update_start() = default;

			bc_event_frame_update_start& operator=(const bc_event_frame_update_start&) = default;
		};

		class bc_event_frame_update_finish : public bc_app_event
		{
			BC_EVENT(fr_up_f)

		public:
			bc_event_frame_update_finish()
				: bc_app_event(message_name())
			{
			}

			bc_event_frame_update_finish(const bc_event_frame_update_finish&) = default;

			~bc_event_frame_update_finish() = default;

			bc_event_frame_update_finish& operator=(const bc_event_frame_update_finish&) = default;
		};

		class bc_event_frame_render_start : public bc_app_render_event
		{
			BC_EVENT(fr_rn_s)

		public:
			bc_event_frame_render_start()
				: bc_app_render_event(message_name())
			{
			}

			bc_event_frame_render_start(const bc_event_frame_render_start&) = default;

			~bc_event_frame_render_start() = default;

			bc_event_frame_render_start& operator=(const bc_event_frame_render_start&) = default;
		};

		class bc_event_frame_render_finish : public bc_app_render_event
		{
			BC_EVENT(fr_rn_f)

		public:
			bc_event_frame_render_finish()
				: bc_app_render_event(message_name())
			{
			}

			bc_event_frame_render_finish(const bc_event_frame_render_finish&) = default;

			~bc_event_frame_render_finish() = default;

			bc_event_frame_render_finish& operator=(const bc_event_frame_render_finish&) = default;
		};

		class bc_event_frame_swap : public bc_app_event
		{
			BC_EVENT(fr_sw)

		public:
			bc_event_frame_swap()
				: bc_app_event(message_name())
			{
			}

			bc_event_frame_swap(const bc_event_frame_swap&) = default;

			~bc_event_frame_swap() = default;

			bc_event_frame_swap& operator=(const bc_event_frame_swap&) = default;
		};
	}
}