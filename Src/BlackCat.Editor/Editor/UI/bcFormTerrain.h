// [02/16/2017 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include <QtWidgets/QtWidgets>

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
				m_material_combo = p_parent.findChild<QComboBox*>("terrainMaterialCombo");
			}

			bcINT32 get_height() const
			{
				return m_height_slider->value();
			}

			bcUINT32 get_radius() const
			{
				return m_radius_slider->value();
			}

			void set_radius(bcUINT32 p_value)
			{
				m_radius_slider->setValue(p_value);
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

			bcUINT32 get_material() const
			{
				return m_material_combo->currentIndex();
			}

			void set_material(bcUINT32 p_index)
			{
				m_material_combo->setCurrentIndex(p_index);
			}

		private:
			QSlider* m_height_slider;
			QSlider* m_radius_slider;
			QSlider* m_smooth_slider;
			QComboBox* m_material_combo;
		};
	}
}