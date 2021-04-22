// [04/22/2021 MRB]

#pragma once

#include "Platform/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_event_editor_mode : public core::bc_app_event
		{
			BC_EVENT(edt_mde)

		public:
			explicit bc_event_editor_mode(bool p_editor_mode) noexcept;

			bc_event_editor_mode(const bc_event_editor_mode&) noexcept;

			~bc_event_editor_mode() override;

			bc_event_editor_mode& operator=(const bc_event_editor_mode&) noexcept;

			bool get_editor_mode() const noexcept;

		private:
			bool m_editor_mode;
		};

		inline bc_event_editor_mode::bc_event_editor_mode(bool p_editor_mode) noexcept
			: bc_app_event(message_name()),
			m_editor_mode(p_editor_mode)
		{
		}

		inline bc_event_editor_mode::bc_event_editor_mode(const bc_event_editor_mode&) noexcept = default;

		inline bc_event_editor_mode::~bc_event_editor_mode() = default;

		inline bc_event_editor_mode& bc_event_editor_mode::operator=(const bc_event_editor_mode&) noexcept = default;

		inline bool bc_event_editor_mode::get_editor_mode() const noexcept
		{
			return m_editor_mode;
		}
	}	
}