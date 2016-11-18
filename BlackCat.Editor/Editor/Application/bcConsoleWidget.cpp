// [11/14/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcConsoleWidget.h"

namespace black_cat
{
	namespace editor
	{
		bc_console_widget::bc_console_widget(QtAwesome* p_awesome, QWidget* p_parent)
			: QWidget(p_parent),
			m_awesome(p_awesome),
			m_info_color(QColor(26, 189, 242)),
			m_debug_color(QColor(251, 160, 28)),
			m_error_color(QColor(250, 106, 106)),
			m_script_color(QColor(174, 174, 174))
		{
			_setup_ui();

			QObject::connect(m_console_input.get(), SIGNAL(returnPressed()), this, SLOT(_returnPressed()));
			QObject::connect(m_toolbar_info.get(), SIGNAL(toggled(bool)), this, SLOT(_infoToggled(bool)));
			QObject::connect(m_toolbar_debug.get(), SIGNAL(toggled(bool)), this, SLOT(_debugToggled(bool)));
			QObject::connect(m_toolbar_error.get(), SIGNAL(toggled(bool)), this, SLOT(_errorToggled(bool)));
			QObject::connect(m_toolbar_script.get(), SIGNAL(toggled(bool)), this, SLOT(_scriptToggled(bool)));
			QObject::connect(m_toolbar_clear.get(), SIGNAL(clicked()), this, SLOT(_clearClicked()));
		}

		bc_console_widget::~bc_console_widget() = default;

		void bc_console_widget::onLog(game::bc_console_output_type p_log_type, const QString& p_log)
		{
			fa::icon l_icon;
			QColor l_color;

			switch (p_log_type)
			{
			case game::bc_console_output_type::info:
				l_icon = fa::infocircle;
				l_color = m_info_color;
				break;
			case game::bc_console_output_type::debug: 
				l_icon = fa::handpointero;
				l_color = m_debug_color;
				break;
			case game::bc_console_output_type::error: 
				l_icon = fa::close;
				l_color = m_error_color;
				break;
			case game::bc_console_output_type::script:
				l_icon = fa::code;
				l_color = m_script_color;
				break;
			default: 
				break;
			}

			QListWidgetItem* l_item = new QListWidgetItem(p_log);
			QVariantMap options;
			options.insert("color", l_color);
			l_item->setIcon(m_awesome->icon(l_icon, options));

			m_console_list->addItem(l_item);
			m_console_list->scrollToBottom();
		}

		void bc_console_widget::onClear()
		{
			m_console_list->clear();
		}

		void bc_console_widget::_returnPressed()
		{
			emit returnPressed(m_console_input->text());
			m_console_input->clear();
		}

		void bc_console_widget::_infoToggled(bool p_toggle)
		{
			if (p_toggle)
			{
				emit uiScripted("console.enableOutput(console.outputInfo)");
			}
			else
			{
				emit uiScripted("console.disableOutput(console.outputInfo)");
			}
		}

		void bc_console_widget::_debugToggled(bool p_toggle)
		{
			if (p_toggle)
			{
				emit uiScripted("console.enableOutput(console.outputDebug)");
			}
			else
			{
				emit uiScripted("console.disableOutput(console.outputDebug)");
			}
		}

		void bc_console_widget::_errorToggled(bool p_toggle)
		{
			if (p_toggle)
			{
				emit uiScripted("console.enableOutput(console.outputError)");
			}
			else
			{
				emit uiScripted("console.disableOutput(console.outputError)");
			}
		}

		void bc_console_widget::_scriptToggled(bool p_toggle)
		{
			if (p_toggle)
			{
				emit uiScripted("console.enableOutput(console.outputScript)");
			}
			else
			{
				emit uiScripted("console.disableOutput(console.outputScript)");
			}
		}

		void bc_console_widget::_clearClicked()
		{
			emit uiScripted("console.clear()");
		}

		void bc_console_widget::_setup_ui()
		{
			m_main_layout = std::make_unique< QVBoxLayout >(nullptr);
			m_main_layout->setMargin(0);
			m_main_layout->setSpacing(0);

			m_toolbar_layout = std::make_unique< QHBoxLayout >(nullptr);
			m_toolbar_layout->setMargin(1);
			m_toolbar_layout->setSpacing(15);
			m_toolbar_layout->addStretch(1);

			m_toolbar = std::make_unique< QFrame >(nullptr);

			QString l_toolbar_button_style = "padding-top: 1px; padding-bottom: 1px;";
			QVariantMap l_info_icon_options;
			l_info_icon_options.insert("color", m_info_color);
			l_info_icon_options.insert("color-selected", m_info_color);
			l_info_icon_options.insert("color-active", m_info_color);
			l_info_icon_options.insert("scale-factor", 0.7);
			QVariantMap l_debug_icon_options;
			l_debug_icon_options.insert("color", m_debug_color);
			l_debug_icon_options.insert("color-selected", m_debug_color);
			l_debug_icon_options.insert("color-active", m_debug_color);
			l_debug_icon_options.insert("scale-factor", 0.7);
			QVariantMap l_error_icon_options;
			l_error_icon_options.insert("color", m_error_color);
			l_error_icon_options.insert("color-selected", m_error_color);
			l_error_icon_options.insert("color-active", m_error_color);
			l_error_icon_options.insert("scale-factor", 0.7);
			QVariantMap l_script_icon_options;
			l_script_icon_options.insert("color", m_script_color);
			l_script_icon_options.insert("color-selected", m_script_color);
			l_script_icon_options.insert("color-active", m_script_color);
			l_script_icon_options.insert("scale-factor", 0.7);

			m_toolbar_info = std::make_unique< QPushButton >(nullptr);
			m_toolbar_info->setText("Info");
			m_toolbar_info->setCheckable(true);
			m_toolbar_info->setChecked(true);
			m_toolbar_info->setStyleSheet(l_toolbar_button_style);
			m_toolbar_info->setIcon(m_awesome->icon(fa::circle, l_info_icon_options));
			m_toolbar_debug = std::make_unique< QPushButton >(nullptr);
			m_toolbar_debug->setText("Debug");
			m_toolbar_debug->setCheckable(true);
			m_toolbar_debug->setChecked(true);
			m_toolbar_debug->setStyleSheet(l_toolbar_button_style);
			m_toolbar_debug->setIcon(m_awesome->icon(fa::circle, l_debug_icon_options));
			m_toolbar_error = std::make_unique< QPushButton >(nullptr);
			m_toolbar_error->setText("Error");
			m_toolbar_error->setCheckable(true);
			m_toolbar_error->setChecked(true);
			m_toolbar_error->setStyleSheet(l_toolbar_button_style);
			m_toolbar_error->setIcon(m_awesome->icon(fa::circle, l_error_icon_options));
			m_toolbar_script = std::make_unique< QPushButton >(nullptr);
			m_toolbar_script->setText("Script");
			m_toolbar_script->setCheckable(true);
			m_toolbar_script->setChecked(true);
			m_toolbar_script->setStyleSheet(l_toolbar_button_style);
			m_toolbar_script->setIcon(m_awesome->icon(fa::circle, l_script_icon_options));
			m_toolbar_clear = std::make_unique< QPushButton >(nullptr);
			m_toolbar_clear->setText("Clear");
			m_toolbar_clear->setStyleSheet(l_toolbar_button_style);
			m_toolbar_clear->setIcon(m_awesome->icon(fa::close));

			m_console_list = std::make_unique< QListWidget >(nullptr);
			m_console_list->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
			m_console_input = std::make_unique< QLineEdit >(nullptr);
			m_console_input->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

			parentWidget()->setLayout(m_main_layout.get());
			m_toolbar->setLayout(m_toolbar_layout.get());

			m_main_layout->addWidget(m_toolbar.get());
			m_main_layout->addWidget(m_console_list.get());
			m_main_layout->addWidget(m_console_input.get());
			m_toolbar_layout->insertWidget(0, m_toolbar_info.get());
			m_toolbar_layout->insertWidget(1, m_toolbar_debug.get());
			m_toolbar_layout->insertWidget(2, m_toolbar_error.get());
			m_toolbar_layout->insertWidget(3, m_toolbar_script.get());
			m_toolbar_layout->insertWidget(4, m_toolbar_clear.get());
		}
	}
}