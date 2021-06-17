// [11/13/2016 MRB]

#pragma once

#include "PlatformImp/Application/bcBasicWindow.h"
#include "GraphicImp/Device/bcDeviceOutput.h"

#pragma once

namespace black_cat
{
	namespace game
	{
		class bci_render_application_output_window
		{
		public:
			virtual ~bci_render_application_output_window() = default;

			virtual platform::bc_window_id get_id() const noexcept = 0;

			virtual bcUINT32 get_width() const noexcept = 0;

			virtual bcUINT32 get_height() const noexcept = 0;

			virtual graphic::bc_device_output get_device_output() const = 0;

			virtual void update() = 0;

			virtual void close() noexcept = 0;

		protected:
			bci_render_application_output_window() = default;

			bci_render_application_output_window(bci_render_application_output_window&&) noexcept = default;

			bci_render_application_output_window& operator=(bci_render_application_output_window&&) noexcept = default;
		};

		class bc_render_application_basic_output_window : public bci_render_application_output_window
		{
		public:
			bc_render_application_basic_output_window(platform::bc_basic_window p_window)
				: m_window(std::move(p_window))
			{
			}

			bc_render_application_basic_output_window(bc_render_application_basic_output_window&&) = default;

			~bc_render_application_basic_output_window() = default;

			bc_render_application_basic_output_window& operator=(bc_render_application_basic_output_window&&) = default;

			platform::bc_basic_window& get_basic_window() noexcept
			{
				return m_window;
			}

			platform::bc_window_id get_id() const noexcept override
			{
				return m_window.get_id();
			}

			bcUINT32 get_width() const noexcept override
			{
				return m_window.get_width();
			}

			bcUINT32 get_height() const noexcept override
			{
				return m_window.get_height();
			}

			graphic::bc_device_output get_device_output() const override
			{
				return graphic::bc_device_output::from_window(const_cast<platform::bc_basic_window&>(m_window));
			}

			void update() override
			{
				return m_window.update();
			}

			void close() noexcept override
			{
				return m_window.close();
			}

		private:
			platform::bc_basic_window m_window;
		};
	}
}