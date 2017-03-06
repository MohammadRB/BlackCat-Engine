// [02/16/2017 MRB]

#pragma once

#include "CorePlatform/bcType.h"

#include <QtWidgets/QMainWindow>
#include <QSlider>

namespace black_cat
{
	namespace editor
	{
		class bc_form_terrain
		{
		public:
			explicit bc_form_terrain(QWidget& p_parent)
			{
				m_height_slider = p_parent.findChild<QSlider*>("terrainHeightSlider");
				m_smooth_slider = p_parent.findChild<QSlider*>("terrainSmoothSlider");
				m_radius_slider = p_parent.findChild<QSlider*>("terrainRadiusSlider");
			}

			bcINT32 get_height() const
			{
				return m_height_slider->value();
			}

			void set_height(bcINT32 p_value)
			{
				m_height_slider->setValue(p_value);
			}

			bcUINT32 get_smooth() const
			{
				return m_smooth_slider->value();
			}

			void set_smooth(bcUINT32 p_value)
			{
				m_smooth_slider->setValue(p_value);
			}

			bcUINT32 get_radius() const
			{
				return m_radius_slider->value();
			}

			void set_radius(bcUINT32 p_value)
			{
				m_radius_slider->setValue(p_value);
			}

		protected:

		private:
			QSlider* m_height_slider;
			QSlider* m_smooth_slider;
			QSlider* m_radius_slider;
		};
	}
}
