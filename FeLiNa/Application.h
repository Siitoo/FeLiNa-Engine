#ifndef _APPLICATION_
#define _APPLICATION_

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleHardware.h"
#include "ModuleConsole.h"
#include "ModuleImport.h"
#include "ModuleTexture.h"
#include "Parson/parson.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleHardware* hardware;
	ModuleConsole* console;
	ModuleImport * mesh_import;
	ModuleTexture* texture;

	char app_name[20];
	char organization[20];

	int FPS_cap = 60;

	bool vsync = false;

private:
	char* name;
	PerfTimer	ms_timer;
	float	dt = 0.0f;
	double last_FPS = 0.0f;
	double last_ms = 0.0f;
	std::list<Module*>list_modules;

	std::vector<float> vector_ms;
	std::vector<float> vector_fps;
	std::vector<float> vector_memory;

	bool need_save = false;
	bool need_load = false;

	bool pause_diagram = false;
public:

	Application();
	~Application();

	bool Init();
	bool Awake();
	update_status Update();
	bool CleanUp();

	float GetMS();
	float GetFPS();

	void DrawApplicationInformationPanel();
	void DrawModulesTime();

	void Log_app(const char* text);

	void NeedSave();
	void NeedLoad();
private:

	void Save();
	void Load();

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif