// [11/16/2018 MRB]

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>

namespace black_cat
{
	namespace editor
	{
		class bc_form_object
		{
		public:
			explicit bc_form_object(QWidget& p_parent)
			{
				m_entity_name_label = p_parent.findChild<QLabel*>("entityNameLabelValue");
			}

			void setEntityName(const QString& p_entity_name)
			{
				m_entity_name_label->setText(p_entity_name);
			}

		protected:

		private:
			QLabel* m_entity_name_label;
		};
	}
}