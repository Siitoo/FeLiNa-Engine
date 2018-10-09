#ifndef _MODULE_FBX_
#define _MODULE_FBX_

#include "Module.h"
#include "OpenGL.h"



class aiScene;
struct aiLogStream;


class ModuleFBX : public Module
{
public:
	ModuleFBX(Application* app, bool start_enabled = true);
	~ModuleFBX();
	bool Start();
	bool CleanUp();

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	GLuint LoadTexture(const char* theFileName);

	void LoadFbx(const char* path);
	

};






#endif