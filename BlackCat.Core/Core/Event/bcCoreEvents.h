// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Event/bcEvent.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		class bc_app_event_error : public bc_app_event
		{
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

			bc_app_event_error(bc_app_event_error&& p_other)
				: bc_app_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_app_event_error() = default;

			bc_app_event_error& operator =(const bc_app_event_error&) = default;

			bc_app_event_error& operator =(bc_app_event_error&& p_other)
			{
				m_message = std::move(p_other.m_message);

				return *this;
			}

			const bc_string& get_message() const noexcept(true)
			{
				return m_message;
			}

			static const bcCHAR* event_name()
			{
				return g_evt_error;
			}

		protected:

		private:
			bc_string m_message;
		};

		class bc_app_event_warning : public bc_app_event
		{
		public:
			explicit bc_app_event_warning(const bcCHAR* p_message)
				: bc_app_event(event_name()),
				m_message(p_message)
			{
			}

			explicit bc_app_event_warning(bc_string p_message)
				: bc_app_event(event_name()),
				m_message(std::move(p_message))
			{
			}

			bc_app_event_warning(const bc_app_event_warning&) = default;

			bc_app_event_warning(bc_app_event_warning&& p_other)
				: bc_app_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_app_event_warning() = default;

			bc_app_event_warning& operator =(const bc_app_event_warning&) = default;

			bc_app_event_warning& operator =(bc_app_event_warning&& p_other)
			{
				m_message = std::move(p_other.m_message);

				return *this;
			}

			const bc_string& get_message() const noexcept(true)
			{
				return m_message;
			}

			static const bcCHAR* event_name()
			{
				return g_evt_warning;
			}

		protected:

		private:
			bc_string m_message;
		};
	}
}