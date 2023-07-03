// [11/2/2015 MRB]

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
			explicit bc_app_event_error(bc_string p_message, bool p_only_log = false)
				: bc_app_event(message_name()),
				m_message(std::move(p_message)),
				m_only_log(p_only_log)
			{
			}

			bc_app_event_error(const bc_app_event_error&) = default;

			bc_app_event_error(bc_app_event_error&& p_other) noexcept(std::is_nothrow_move_constructible_v<bc_string>)
				: bc_app_event(p_other),
				m_message(std::move(p_other.m_message)),
				m_only_log(p_other.m_only_log)
			{
			}

			~bc_app_event_error() override = default;

			bc_app_event_error& operator =(const bc_app_event_error&) = default;

			bc_app_event_error& operator =(bc_app_event_error&& p_other) noexcept(std::is_nothrow_move_assignable_v< bc_string >)
			{
				m_message = std::move(p_other.m_message);
				m_only_log = p_other.m_only_log;

				return *this;
			}

			bool get_only_log() const noexcept
			{
				return m_only_log;
			}

			bc_string_view get_message() const noexcept
			{
				return m_message;
			}

		private:
			bc_string m_message;
			bool m_only_log;
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

			~bc_event_frame_update_start() override = default;

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

			~bc_event_frame_update_finish() override = default;

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

			~bc_event_frame_render_start() override = default;

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

			~bc_event_frame_render_finish() override = default;

			bc_event_frame_render_finish& operator=(const bc_event_frame_render_finish&) = default;
		};

		class bc_event_frame_swap : public bc_app_event
		{
			BC_EVENT(fr_sw)

		public:
			explicit bc_event_frame_swap(bcFLOAT p_fps)
				: bc_app_event(message_name()),
				m_fps(p_fps)
			{
			}

			bc_event_frame_swap(const bc_event_frame_swap&) = default;

			~bc_event_frame_swap() override = default;

			bc_event_frame_swap& operator=(const bc_event_frame_swap&) = default;

			bcFLOAT get_fps() const noexcept
			{
				return m_fps;
			}
			
		private:
			bcFLOAT m_fps;
		};
	}
}