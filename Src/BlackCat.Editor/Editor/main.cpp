// [11/12/2016 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcEditorApplication.h"
#include <QtWidgets/QApplication>

using namespace black_cat::editor;

int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, CHAR* p_cmd_line, int p_cmd_show)
{
	int l_commandline_count = 0;
	char** l_commandline = nullptr;
	
    QApplication l_q_app(l_commandline_count, l_commandline);
	bc_editor_app l_editor_app(p_instance);

	l_editor_app.show();
    return l_q_app.exec();
}