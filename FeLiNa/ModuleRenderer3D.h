#ifndef _MODULE_RENDER_3D_
#define _MODULE_RENDER_3D_

#include "Module.h"
#include "Light.h"
#include "Assimp/include/cfileio.h"
#include "MathGeoLib/Math/Quat.h"
#define MAX_LIGHTS 8

class ModuleImage;
class ComponentMesh;
struct Mesh
{
	std::string name = "";
	std::string path = "";

	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;
};


class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Awake(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	float4x4 perspective(float fovy, float aspect, float n, float f);

	void OnResize(int width, int height);

	void DrawCheckBoxEdgeGLPanel();

	void DrawMesh(Mesh *mesh);

	void AddDataMesh(Mesh* data_mesh);
	void DeleteAllDataMesh();
	void DrawMeshInformation();
	void CreateCheckers();
	void CleanAllDataModel();
	void SaveState(JSON_Object* config);

	ComponentMesh* CreateComponentMesh();
public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	std::vector<Mesh*> meshes;
	bool wire = false;
private:
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;;
	bool texture2D = false;
	bool line_smooth = false;
	bool polygon_smooth = false;
	
	ModuleImage* img = nullptr;

	//Checkers
	uint checker_id = 0;
	bool material_cheker = false;
	bool no_texture = false;

};

#endif