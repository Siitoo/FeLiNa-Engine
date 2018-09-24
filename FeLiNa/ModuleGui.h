#ifndef _MODULEGUI_H
#define _MODULEGUI_H

#include "Module.h"
#include "Globals.h"

#include "imgui-1.65/imgui.h"
#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"

class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:
	void ShowMainMenuBar();

public:
	//Variables to main menu bar
	bool close_program = false;


	//Variables to open windows

	
};


#endif
