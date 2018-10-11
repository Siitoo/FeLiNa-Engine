#include "ModuleImport.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")
#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )
#include <string>

ModuleImport::ModuleImport(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleImport::~ModuleImport()
{

}

void myCallback(const char *msg, char *userData) {
	LOG("%s" ,msg);
}

bool ModuleImport::Start()
{
	aiLogStream stream = aiLogStream();
	stream.callback = myCallback;
	aiAttachLogStream(&stream);
	ilInit();

	return true;
}

bool ModuleImport::CleanUp()
{
	aiDetachAllLogStreams();

	//TO CHANGE
	/*if (data->indices != nullptr)
		delete[]data->indices;

	if (data->vertices != nullptr)
		delete[]data->vertices;*/

	return true;
}

void ModuleImport::LoadData(const char* path)
{
    LOG("Inicialization load data model")

	ModelData* data = new ModelData();

	//TO REVISION
	data->path = path;
	std::string tmp = path;
	data->name = tmp.erase(0,tmp.find_last_of("\\") + 1);


	const aiScene* scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		
			for (int num_meshes = 0; num_meshes < scene->mNumMeshes; ++num_meshes)
			{
				

				aiMesh* new_mesh = scene->mMeshes[num_meshes];
				data->num_vertices = new_mesh->mNumVertices;
				data->vertices = new float[data->num_vertices * 3];
				memcpy(data->vertices, new_mesh->mVertices, sizeof(float)*data->num_vertices * 3);
				LOG("New mesh with %d verices", data->num_vertices);

				//Geometry

				if (new_mesh->HasFaces())
				{
					data->num_indices = new_mesh->mNumFaces * 3;
					data->indices = new uint[data->num_indices];

					for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
					{
						if (new_mesh->mFaces[num_faces].mNumIndices != 3)
						{
							LOG("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
						}
						else
							memcpy(&data->indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

					}
				}
				
				//TO REVISION -> ALL WITH TEXTURE---------------------------------------------
				///Only 1 material for now
				aiMaterial* material = scene->mMaterials[0];

				if (material != nullptr)
				{
					//TO REVISION------------------------------------------------------------------------------------------
					aiString texture_name;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

					std::string fbx_path = path;

					std::string fbx_folder = fbx_path.substr(0,fbx_path.find_last_of("\\")+1)+ texture_name.data;
					std::string game_folder = fbx_path.substr(0, fbx_path.find("Game\\")+5) + texture_name.data; // 5 = Game/
					std::string felina_folder = fbx_path.substr(0,fbx_path.find("FeLiNa\\") + 7) + texture_name.data;
					
					uint tex_id = 0;
					tex_id = App->texture->LoadTexture(fbx_folder.c_str());

					if (tex_id == 0)
					{
						tex_id = App->texture->LoadTexture(game_folder.c_str());

						if (tex_id == 0)
						{
							tex_id = App->texture->LoadTexture(felina_folder.c_str());

						}
					}

					if (tex_id != 0)
					{
						data->texture_id = tex_id;

					}
					//-----------------------------------------------------------------------------------------------
				}


				///Only 1 material for now
				if (new_mesh->HasTextureCoords(0))
				{
					data->num_uv = new_mesh->mNumVertices;
					data->uv = new float[data->num_uv * 2];

					for (uint coords = 0; coords < new_mesh->mNumVertices; ++coords)
					{
						memcpy(&data->uv[coords * 2], &new_mesh->mTextureCoords[0][coords].x, sizeof(float));
						memcpy(&data->uv[(coords * 2) + 1], &new_mesh->mTextureCoords[0][coords].y, sizeof(float));
					}
				}
				//------------------------------------------------------------------------------
				
				//TO REVISION--------------------------------------------------------

				/*aiMaterial* color_material = scene->mMaterials[new_mesh->mMaterialIndex];

				// if the object don't have color material, we set the color to white 
				//because if not, the object take the last material
				if (aiGetMaterialColor(color_material, AI_MATKEY_COLOR_AMBIENT, &data->color_4D) == aiReturn_FAILURE || data->color_4D == aiColor4D(0,0,0,1))
				{
					data->color_4D = { 255.0f,255.0f,255.0f,255.0f }; 
				}
				aiColor4D* colors_mesh = *new_mesh->mColors;

				if (colors_mesh != nullptr)
				{
					data->colors = new float[data->num_vertices * 3];
					for (int num_color = 0; num_color < data->num_vertices; ++num_color)
					{
						memcpy(data->colors, &colors_mesh[num_color], sizeof(float)*data->num_vertices * 3);
					}
				}*/
				//--------------------------------------------------------------------------------


				glGenBuffers(1, (GLuint*) &(data->id_vertices));
				glBindBuffer(GL_ARRAY_BUFFER, data->id_vertices);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * data->num_vertices, data->vertices, GL_STATIC_DRAW);



				glGenBuffers(1, (GLuint*) &(data->id_indices));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->id_indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->num_indices, data->indices, GL_STATIC_DRAW);


				glGenBuffers(1, (GLuint*) &(data->id_uv));
				glBindBuffer(GL_ARRAY_BUFFER, data->id_uv);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * data->num_uv, data->uv, GL_STATIC_DRAW);



				/*glGenBuffers(1, (GLuint*) &(data->id_color));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->id_color);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->num_color, data->colors, GL_STATIC_DRAW);*/

				App->renderer3D->AddDataMesh(data);
			}


			
	}

	

	else
		LOG("Error loading Scene %s",path);
}

update_status ModuleImport::PreUpdate(float dt) {
	update_status update_return = UPDATE_CONTINUE;
	module_timer.Start();
	return update_return;
}

update_status ModuleImport::PostUpdate(float dt) {

	update_status update_return = UPDATE_CONTINUE;

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

	return update_return;

}
