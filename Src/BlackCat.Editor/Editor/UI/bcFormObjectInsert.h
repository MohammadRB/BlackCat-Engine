// [11/21/2018 MRB]

#pragma once

#include <QtWidgets/QListWidget>

namespace black_cat
{
	namespace editor
	{
		class bc_form_object_insert
		{
		public:
			bc_form_object_insert(QWidget& p_parent)
			{
				m_entity_list = p_parent.findChild<QListWidget*>("entityNameList");
			}

			void add_entity_names(QVector<QString> p_entity_names, bool p_clear_exists = true)
			{
				if(p_clear_exists)
				{
					m_entity_list->clear();
				}

				for(auto& l_entity : p_entity_names)
				{
					m_entity_list->addItem(l_entity);
				}
			}

			QString get_selected_entity() const
			{
				auto* selected_item = m_entity_list->currentItem();
				if(selected_item == nullptr)
				{
					return QString();
				}

				return selected_item->text();
			}

		protected:

		private:
			QListWidget* m_entity_list;
		};
	}
}