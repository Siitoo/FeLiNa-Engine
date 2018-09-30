#include "ModuleConsole.h"


ModuleConsole::ModuleConsole(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleConsole::~ModuleConsole()
{

}

void ModuleConsole::Log_console(const char* text) {

	console_buffer.appendf(text);
	console_scroll = true;
	
}

void ModuleConsole::DrawConsole() {

	ImGui::SetNextWindowSize({ 522,300 });
	ImGui::SetNextWindowBgAlpha(0.2);

	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("Console", &Log_active, window_flags);
	ImGui::TextUnformatted(console_buffer.begin());
	if (console_scroll)
		ImGui::SetScrollHere(1.0f);
	console_scroll = false;
	ImGui::End();
}