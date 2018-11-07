#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "MeshImporter.h"
#include "ImGui/imgui.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;

	mesh = new Mesh();

}

ComponentMesh::~ComponentMesh()
{
	mesh = nullptr;
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
		ImGui::Text("Indices: %i", mesh->num_indices);
		ImGui::Text("Vertices: %i", mesh->num_vertices);
		ImGui::Text("Uv's: %i", mesh->num_uv);
		ImGui::Text("Triangles: %i", mesh->num_vertices/3);

		ImGui::TreePop();
	}
}

void ComponentMesh::SetPath(char* path) {

	mesh->felina_path = path;

}

void ComponentMesh::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_string(obj, "path", mesh->felina_path);
}

void ComponentMesh::OnLoad(JSON_Object* obj)
{
	memcpy(mesh->felina_path, (char*)json_object_get_string(obj, "path"), DEFAULT_BUF_SIZE);
}