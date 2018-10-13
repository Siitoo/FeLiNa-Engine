#ifndef _MODULE_HARDWARE_
#define _MODULE_HARDWARE_

#include "Module.h"
#include "SDL/include/SDL_opengl.h"


class ModuleHardware : public Module
{
public:
	ModuleHardware(Application* parent, bool start_enabled = true);
	~ModuleHardware();

	void DrawHardwareInformationPanel();

	int devil_current_version_num = 0;


private:
	void FillHardwareInfo();

	char sdl_current_version[10];
	char devil_current_version[10];
	char glew_current_version[10];
	char assimp_current_version[10];

	//CPU
	int cpu_count = 0;
	char cpu_version_count[10];
	int total_ram = 0;
	char total_ram_count[10];
	bool RDTSC = false;
	bool MMX = false;
	bool AVX = false;
	bool AVX2 = false;
	bool SSE = false;
	bool SSE2 = false;
	bool SSE3 = false;
	bool SSE41 = false;
	bool SSE42 = false;
	//GPU
	const GLubyte *vendor;
	const GLubyte *gpu_name;
	GLint total_memory = 0;
	GLint memory_usage = 0;
	GLint dedicated_memory = 0;
	GLint available_memory = 0;

	bool fill_hardware = false;
};



#endif


