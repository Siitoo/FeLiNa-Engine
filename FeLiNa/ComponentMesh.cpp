#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "MeshImporter.h"
#include "ImGui/imgui.h"
#include "Resource.h"
#include "Application.h"
#include "ResourceManager.h"
#include "ResourceMesh.h"
#include "mmgr/mmgr.h"
ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;

	mesh = new Mesh();
	mesh->felina_path = new char[DEFAULT_BUF_SIZE];
}

ComponentMesh::~ComponentMesh()
{
	uint uid = App->resource_manager->Find(mesh->felina_path);

	if (uid != 0)
	{
		ResourceMesh* resource_mesh = (ResourceMesh*)App->resource_manager->Get(uid);
		resource_mesh->EraseToMemory();
	}
}


void ComponentMesh::CleanUp()
{
	uint uid = App->resource_manager->Find(mesh->felina_path);

	if (uid != 0)
	{
		ResourceMesh* resource_mesh = (ResourceMesh*)App->resource_manager->Get(uid);
		resource_mesh->EraseToMemory();
	}

}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

Mesh* ComponentMesh::GetMesh() const
{
	return mesh;
}

void ComponentMesh::DrawInspector()
{
	if (ImGui::TreeNodeEx("Mesh"))
	{
		uint uid = App->resource_manager->Find(mesh->felina_path);
		if (uid != 0)
		{
			Resource* resource = App->resource_manager->Get(uid);

			ImGui::Text("Refernce counting: %i", resource->loaded);
		}
		ImGui::Text("Indices: %i", mesh->num_indices);
		ImGui::Text("Vertices: %i", mesh->num_vertices);
		ImGui::Text("Uv's: %i", mesh->num_uv);
		ImGui::Text("Triangles: %i", mesh->num_vertices/3);

		ImGui::TreePop();
	}
}

void ComponentMesh::SetPath(char* path) {

	
	strcpy(mesh->felina_path, path);

}

void ComponentMesh::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_string(obj, "path", mesh->felina_path);
}

void ComponentMesh::OnLoad(JSON_Object* obj)
{
	char* tmp = (char*)json_object_get_string(obj, "path");
	uint uid = App->resource_manager->ImportOwnFile(tmp);

	ResourceMesh* resource_mesh = (ResourceMesh*)App->resource_manager->Get(uid);
	
	
	mesh = resource_mesh->GetMesh();
	
	parent->bounding_box.Enclose((math::float3 *)mesh->vertices, mesh->num_vertices);
	
	parent->RecalculateBoundingBox();
}