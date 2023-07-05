// [10/12/2021 MRB]

#pragma once

#include <QtWidgets/QListWidget>

namespace black_cat::editor
{
	class bc_form_decal_insert
	{
	public:
		explicit bc_form_decal_insert(QWidget& p_parent)
		{
			m_decal_list = p_parent.findChild<QListWidget*>("decalNameList");
		}

		void add_decal_names(const QVector<QString>& p_decal_names, bool p_clear_exists = true)
		{
			if (p_clear_exists)
			{
				m_decal_list->clear();
			}

			for (auto& l_entity : p_decal_names)
			{
				m_decal_list->addItem(l_entity);
			}
		}

		QString get_selected_decal() const
		{
			auto* l_selected_item = m_decal_list->currentItem();
			if (l_selected_item == nullptr)
			{
				return QString();
			}

			return l_selected_item->text();
		}

	private:
		QListWidget* m_decal_list;
	};
}
