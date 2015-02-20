// [2/11/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Event/bcEvent.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_error : public bc_event
		{
		public:
			bc_event_error(const bcCHAR* p_message)
				: bc_event("error"),
				m_message(p_message)
			{
			}

			bc_event_error(bc_string p_message)
				: bc_event("error"),
				m_message(std::move(p_message))
			{
			}

			bc_event_error(const bc_event_error&) = default;

			bc_event_error(bc_event_error&& p_other)
				: bc_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_event_error() = default;

			bc_event_error& operator =(const bc_event_error&) = default;

			bc_event_error& operator =(bc_event_error&& p_other)
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

		class bc_event_warning : public bc_event
		{
		public:
			bc_event_warning(const bcCHAR* p_message)
				: bc_event("warning"),
				m_message(p_message)
			{
			}

			bc_event_warning(bc_string p_message)
				: bc_event("warning"),
				m_message(std::move(p_message))
			{
			}

			bc_event_warning(const bc_event_warning&) = default;

			bc_event_warning(bc_event_warning&& p_other)
				: bc_event(p_other),
				m_message(std::move(p_other.m_message))
			{
			}

			~bc_event_warning() = default;

			bc_event_warning& operator =(const bc_event_warning&) = default;

			bc_event_warning& operator =(bc_event_warning&& p_other)
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
	}
}