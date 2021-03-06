#include "Application.h"
#include "MaterialImporter.h"
#include "MeshImporter.h"
#include "ModuleRenderer3D.h"
#include "ModuleHardware.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleConsole.h"
#include "ModuleFileSystem.h"
#include "ModuleTimeManagement.h"
#include "SceneSerialization.h"
#include "ModuleResourceManager.h"
#include "PCG/entropy.h"
#include "mmgr/mmgr.h"

Application::Application()
{
	name = "Application";
	
	app_name = new char[DEFAULT_BUF_SIZE];
	organization = new char[DEFAULT_BUF_SIZE];

	strcpy_s(app_name, DEFAULT_BUF_SIZE, "FeLiNa Engine");
	strcpy_s(organization, DEFAULT_BUF_SIZE, "CITM UPC");
	

#ifndef GAME_MODE
	
	hardware = new ModuleHardware(this, false);
	gui = new ModuleGui(this);
	console = new ModuleConsole(this);
	time_management = new ModuleTimeManagement(this);
#endif // !GAME_MODE

	resource_manager = new ModuleResourceManager(this);
	serialization_scene = new SceneSerialization();
	importer_material = new MaterialImporter();
	importer_mesh = new MeshImporter();
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	fs = new ModuleFileSystem(this);
	
	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(fs);
	AddModule(resource_manager);
	// Scenes
	AddModule(scene);

#ifndef GAME_MODE
	AddModule(console);
	AddModule(hardware);
	AddModule(time_management);
	AddModule(gui);
#endif 

	AddModule(renderer3D);
}

Application::~Application()
{

	for (std::list<Module*>::const_reverse_iterator it = list_modules.rbegin(); it != list_modules.rend(); ++it)
	{
		delete *it;
	}

#ifndef GAME_MODE
	RELEASE(importer_mesh);
	RELEASE(importer_material);
#endif // !GAME_MODE


	RELEASE_ARRAY(name);
	RELEASE_ARRAY(app_name);
	RELEASE_ARRAY(organization);
	RELEASE_ARRAY(name);
	RELEASE_ARRAY(organization);
}

bool Application::Init()
{
	bool ret = true;

	uint64_t seeds[2];
	entropy_getbytes((void*)seeds, sizeof(seeds));
	pcg32_srandom_r(&random, seeds[0], seeds[1]);


	// Call Init() in all modules
	for (std::list<Module*>::const_iterator it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::const_iterator it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Start();
	}

	return ret;
}


bool Application::Awake()
{
	bool ret = true;

	char* buffer;
	uint size = fs->Load("config.json",&buffer);


	if (size >0)
	{
		JSON_Value* node = json_parse_string(buffer);

		JSON_Object* config_app = json_value_get_object(node);

		if (config_app != nullptr)
		{
			JSON_Object* app_object = json_object_get_object(config_app, "Application");
			
			strcpy_s(app_name, DEFAULT_BUF_SIZE, json_object_get_string(app_object, "Title"));
			strcpy_s(organization,DEFAULT_BUF_SIZE ,(char*)json_object_get_string(app_object, "Organization"));

			vsync = json_object_get_boolean(app_object, "VSYNC");
			FPS_cap = json_object_get_number(app_object, "Max frames");
		}

		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
		{
			JSON_Object* module_obj = json_object_get_object(config_app, (*it)->GetName());

			ret = (*it)->Awake(module_obj);
		}
		
		json_value_free(node);

	}

	RELEASE_ARRAY(buffer);

	return true;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	ms_timer.Start();


	switch (engine_states)
	{
	case ENGINE_STATE_EDITOR_MODE:

		if (game_states == ENGINE_STATE_PLAY) {
			
			engine_states = ENGINE_STATES::ENGINE_STATE_GAME_MODE;

		}

		break;

	case ENGINE_STATE_GAME_MODE:


		if (game_states == ENGINE_STATE_STOP) {
			engine_states = ENGINE_STATES::ENGINE_STATE_EDITOR_MODE;
			camera->current_camera = camera->camera_editor;

			serialization_scene->save_name_scene = "auto";
			serialization_scene->ClearActualScene();
			serialization_scene->LoadScene(serialization_scene->save_name_scene);
		}

		break;
	}
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	last_ms = ms_timer.ReadMs();

	if (!vsync) {

		double ms_cap = 0.0f;

		if (FPS_cap > 0) {
			ms_cap = 1000.0f / FPS_cap;
		}

		else {
			ms_cap = 1000.0f / 60;
		}
		if (last_ms < ms_cap) {
			SDL_Delay(ms_cap - last_ms);
		}

	}
#ifndef GAME_MODE
	App->time_management->FinishUpdate();
#endif
	last_ms = ms_timer.ReadMs();
	last_FPS = 1000.0 / last_ms;
	dt = 1 / last_FPS;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::const_iterator it = list_modules.begin();
	
	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*it)->module_timer.Start();
		ret = (*it)->PreUpdate(dt);
		(*it)->module_timer.Pause();
		++it;
	}

	it = list_modules.begin();

	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*it)->module_timer.Play();
		ret = (*it)->Update(dt);
		(*it)->module_timer.Pause();
		++it;
	}

	it = list_modules.begin();

	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*it)->module_timer.Play();
		ret = (*it)->PostUpdate(dt);
		if (!pause_diagram) {

			(*it)->last_update_ms = (*it)->module_timer.ReadMs();
			
			if ((*it)->vector_update_ms.size() >= 100) {
				
				(*it)->vector_update_ms.erase((*it)->vector_update_ms.begin());
			
			}
			
			(*it)->vector_update_ms.push_back((*it)->last_update_ms);
		}


		++it;
	}
	
	FinishUpdate();

	if (need_save)
		Save();

	if (need_load)
		Load();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	Save();

	for (std::list<Module*>::const_reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

float Application::GetMS() const
{
	return last_ms;
}

float Application::GetFPS() const
{
	return last_FPS;
}

uint Application::GenerateRandomNumber() const
{
	uint ret = pcg32_random_r(&(App->random));

	while (ret > 2147483647) { //if not, the capacity of int are passed
		ret = pcg32_random_r(&(App->random));
	}

	return ret;
}


void Application::Save()
{
#ifndef GAME_MODE
	Log_app("Saving State....");
#endif

	//Create a new object
	JSON_Value* root = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root);
	
	//Create a new object to store values
	JSON_Value* new_value = json_value_init_object();
	JSON_Object* new_object = json_value_get_object(new_value);
	json_object_set_value(root_object, "Application", new_value);

	json_object_set_string(new_object,"Title", app_name);
	json_object_set_string(new_object, "Organization", organization);
	json_object_set_boolean(new_object, "VSYNC", vsync);
	json_object_set_number(new_object, "Max Frames", FPS_cap);
	
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		 new_value = json_value_init_object();
		 new_object = json_value_get_object(new_value);

		json_object_set_value(root_object, (*it)->GetName(), new_value);


		(*it)->SaveState(new_object);
	}


	uint size = json_serialization_size_pretty(root);
	char* buffer = new char[size];

	json_serialize_to_buffer_pretty(root, buffer, size);

	fs->SaveBufferData(buffer, "config.json", size);

	json_value_free(root);

	RELEASE_ARRAY(buffer);

	need_save = false;
}

void Application::NeedSave()
{
	need_save = true;
}

void Application::NeedLoad()
{
	need_load = true;
}

void Application::Load()
{
#ifndef GAME_MODE
	Log_app("Loading State....");
#endif
	JSON_Value* root = nullptr;
	root = json_parse_file("data.json");


	if (root != nullptr)
	{
		
		JSON_Object* data = json_value_get_object(root);

		if (data != nullptr)
		{
			JSON_Object* config_app = json_object_get_object(data, name);

			if (config_app != nullptr)
			{
				strcpy(app_name, json_object_get_string(config_app, "Title"));
				strcpy(organization, json_object_get_string(config_app, "Organization"));
				vsync = json_object_get_boolean(config_app, "VSYNC");
				FPS_cap = json_object_get_number(config_app, "Max frames");


				for (std::list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); ++item)
				{
					JSON_Object* module_to_save = json_object_get_object(data, (*item)->GetName());
					(*item)->LoadState(module_to_save);
				}
			}
		}
		json_value_free(root);

#ifndef GAME_MODE
		Log_app("Load succesful");
#endif
	}
#ifndef  GAME_MODE
	else
		Log_app("Load failed: can't find root");
#endif // ! GAME_MODE

	need_load = false;
}

#ifndef GAME_MODE
void Application::DrawApplicationInformationPanel()
{
	if (ImGui::InputText("App name", app_name, 20))
	{
		window->SetTitle(app_name);
	}
		
	ImGui::InputText("Organization", organization, 20);

	if (!pause_diagram) {
		if (vector_fps.size() != 100)
		{

			vector_fps.push_back(GetFPS());
			vector_ms.push_back(GetMS());
		}
		else
		{
			vector_fps.erase(vector_fps.begin());
			vector_fps.push_back(GetFPS());

			vector_ms.erase(vector_ms.begin());
			vector_ms.push_back(GetMS());
		}
	}
	char title[30];

	ImGui::SliderInt("FPS", &FPS_cap, 0, 120);
	
	ImGui::Checkbox("Vsync", &vsync);
	ImGui::SameLine();
	ImGui::Text("(Restart to apply changes)");

	ImGui::Checkbox("Pause", &pause_diagram);
	
	sprintf_s(title, 25, "Framerate %.1f", vector_fps[vector_fps.size() - 1]);
	ImGui::PlotHistogram("##framerate", &vector_fps[0], vector_fps.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sprintf_s(title, 25, "Milliseconds %.1f", vector_ms[vector_ms.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &vector_ms[0], vector_ms.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sMStats stats = m_getMemoryStatistics();

	if (vector_memory.size() != 100)
	{
		vector_memory.push_back((int)stats.accumulatedActualMemory);
	}
	else
	{
		vector_memory.erase(vector_memory.begin());
		vector_memory.push_back((int)stats.accumulatedActualMemory);
	}

	sprintf_s(title, 25, "Memory Consumption ");
	ImGui::PlotHistogram("##Memory Consumption", &vector_memory[0], vector_memory.size(), 0, title, 0.0f, 100000000, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.totalReportedMemory);

	ImGui::Text("Total Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedActualMemory);

	ImGui::Text("Peak Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakReportedMemory);

	ImGui::Text("Peak Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakActualMemory);

	ImGui::Text("Accumulated Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedReportedMemory);

	ImGui::Text("Accumulated Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedActualMemory);

	ImGui::Text("Accumulated Alloc Unit Count:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedAllocUnitCount);

	ImGui::Text("Total Alloc Unit Count:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.totalAllocUnitCount);

	ImGui::Text("Peak Alloc Unit Count");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakAllocUnitCount);

	
}


void Application::DrawModulesTime() {

	ImGui::Text("Module Camera 3D");
	ImGui::PlotHistogram("##milliseconds", &camera->vector_update_ms[0], camera->vector_update_ms.size(), 0, "MS", 0.0f, 1.0f, ImVec2(310, 100));

	ImGui::Text("Module GUI");
	ImGui::PlotHistogram("##milliseconds", &gui->vector_update_ms[0], gui->vector_update_ms.size(), 0, "MS", 0.0f, 1.0f, ImVec2(310, 100));

	ImGui::Text("Module Input");
	ImGui::PlotHistogram("##milliseconds", &input->vector_update_ms[0], input->vector_update_ms.size(), 0, "MS", 0.0f, 1.0f, ImVec2(310, 100));

	ImGui::Text("Module Renderer");
	ImGui::PlotHistogram("##milliseconds", &renderer3D->vector_update_ms[0], renderer3D->vector_update_ms.size(), 0, "MS", 0.0f, 1.0f, ImVec2(310, 100));

	ImGui::Text("Module Scene Intro");
	ImGui::PlotHistogram("##milliseconds", &scene->vector_update_ms[0], scene->vector_update_ms.size(), 0, "MS", 0.0f, 1.0f, ImVec2(310, 100));

}


void Application::Log_app(const char * text) const
{
	console->Log_console(text);

}

#endif

