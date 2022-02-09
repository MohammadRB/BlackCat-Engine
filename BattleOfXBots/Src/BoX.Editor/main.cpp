// [01/29/2022 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/Application/bcEditorApplication.h"
#include "BoX.Editor/Application/bxEditorRenderApplication.h"
#include <QtWidgets/QApplication>

int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, CHAR* p_cmd_line, int p_cmd_show)
{
	int l_commandline_count = 0;
	char** l_commandline = nullptr;

	QApplication l_q_app(l_commandline_count, l_commandline);
	box::bx_editor_render_app l_render_app;
	black_cat::editor::bc_editor_app l_editor_app(p_instance, p_cmd_line, [&]() {return &l_render_app; });

	l_editor_app.show();
	return l_q_app.exec();
}