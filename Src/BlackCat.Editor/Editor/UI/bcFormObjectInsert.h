// [21/11/2018 MRB]

#pragma once

#include <QtWidgets/QListWidget>

namespace black_cat::editor
{
	class bc_form_object_insert
	{
	public:
		explicit bc_form_object_insert(QWidget& p_parent)
		{
			m_entity_list = p_parent.findChild<QListWidget*>("entityNameList");
		}

		void add_entity_names(const QVector<QString>& p_entity_names, bool p_clear_exists = true)
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
			auto* l_selected_item = m_entity_list->currentItem();
			if(l_selected_item == nullptr)
			{
				return QString();
			}

			return l_selected_item->text();
		}

	private:
		QListWidget* m_entity_list;
	};
}
