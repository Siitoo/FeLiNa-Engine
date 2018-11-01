#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "ModuleScene.h"
#include "Glew/include/glew.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"
#include "Quadtree.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"

GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;

	if (parent != nullptr)
		parent->childrens.push_back(this);

	

	bounding_box.SetNegativeInfinity();

	uid = App->random->Int();
	
}


GameObject::~GameObject()
{
	for (std::vector<GameObject*>::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	delete name;
	name = nullptr;

	delete parent;
	parent = nullptr;
}

void GameObject::Update(float dt)
{
	
	for (int i = 0; i < childrens.size(); ++i)
		childrens[i]->Update(dt);

	for (int i = 0; i < components.size(); ++i)
		components[i]->Update(dt);

}

bool GameObject::CleanUp()
{
	for (std::vector<GameObject*>::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	delete name;
	name = nullptr;

	delete parent;
	parent = nullptr;

	return true;
}


void GameObject::SetName(char* name)
{
	this->name = name;
	
}

char* GameObject::GetName() const
{
	return name;
}

void GameObject::SetActive(bool* active)
{
	this->active = active;
}

bool GameObject::GetActive() const
{
	return active;
}

void GameObject::AddChildren(GameObject* child)
{
	childrens.push_back(child);
}

GameObject* GameObject::GetChild(uint position)
{
	return childrens[position];
}

void GameObject::SetParent(GameObject* parent)
{
	this->parent = parent;
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetComponent(Component* component)
{
	if (component != nullptr)
	{
		component->SetParent(this);
		components.push_back(component);

		
	}
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;

	switch (type)
	{
	case Component_Transform:
		component = new ComponentTransform(this);
		break;
	case Component_Mesh:
		component = new ComponentMesh(this);
		break;
	case Component_Material:
		component = new ComponentTexture(this);
		break;
	case Component_Camera:
		component = new ComponentCamera(this);
		break;

	case Component_Default:
		break;
	default:
		break;
	}

	components.push_back(component);

	return component;
}

bool GameObject::DeleteComponent(Component* component)
{
	bool ret = false;

	if (components.size() != 0)
	{

		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it) == component)
			{
				(*it)->CleanUp();
				components.erase(it);

				ret = true;
			}

		}

		components.shrink_to_fit();
	}

	return ret;
}

bool GameObject::DeleteComponent(ComponentType type)
{
	bool ret = false;

	if (components.size() != 0)
	{

		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->GetComponentType() == type)
			{
				(*it)->CleanUp();
				components.erase(it);

				ret = true;
			}

		}

		components.shrink_to_fit();
	}

	return ret;
}

uint GameObject::GetNumChildren() const
{
	return childrens.size();
}

void GameObject::ShowObjectHierarchy()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	bool node_open = false;

	if (selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (childrens.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx(name, flags))
		node_open = true;

	if (ImGui::IsItemClicked())
	{
		GameObject* go = App->scene->GetSelectedGameObject();

		if (go != nullptr)
		{
			go->SetSelected(false);
		}

		App->scene->SetSelectedGameObject(this);
		SetSelected(true);
	}

	if (node_open)
	{
		for (uint i = 0; i < childrens.size(); ++i)
		{
			ImGui::PushID(i);
			childrens[i]->ShowObjectHierarchy();
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
	
}

void GameObject::SetSelected(bool selected)
{
	this->selected = selected;
}

bool GameObject::IsSelected() const
{
	return selected;
}

void GameObject::ToggleSelected()
{
	if (selected)
		selected = false;
	else
		selected = true;
}

void GameObject::ShowObjectInspector()
{

	if (ImGui::TreeNodeEx("Properties:"))
	{

		if (ImGui::Checkbox("Static", &static_object))
		{
			if (static_object)
				App->scene->static_go.push_back(this);
				
			App->scene->need_update_quadtree = true;
		}

		ImGui::InputText("##name", name, 30);

		ImGui::TreePop();
	}


	for (int i = 0; i < components.size(); ++i)
	{
		ImGui::PushID(i);
		components[i]->DrawInspector();
		ImGui::PopID();
	}

}

void GameObject::AddBoundingBox(const Mesh* mesh)
{
	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose((math::float3 *)mesh->vertices,mesh->num_vertices);
}

void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(bounding_box.Edge(i).a.x, bounding_box.Edge(i).a.y, bounding_box.Edge(i).a.z);
		glVertex3f(bounding_box.Edge(i).b.x, bounding_box.Edge(i).b.y, bounding_box.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	

}


Component* GameObject::GetComponent(ComponentType type)
{
	Component* component = nullptr;

	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}

void GameObject::RecalculateBoundingBox()
{
	bounding_box.SetNegativeInfinity();

	ComponentTransform* tr = (ComponentTransform*)GetComponent(Component_Transform);
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(Component_Mesh);


	if (mesh != nullptr)
		bounding_box.Enclose((const math::float3*)mesh->GetMesh()->vertices, mesh->GetMesh()->num_vertices);

	if (tr != nullptr)
	{
		math::OBB obb = bounding_box.Transform(tr->GetTransformMatrix());

		if (obb.IsFinite())
			bounding_box = obb.MinimalEnclosingAABB();
	}

	for (uint i = 0; i < childrens.size(); ++i)
		childrens[i]->RecalculateBoundingBox();
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

bool GameObject::IsActive() const
{
	return active;
}

math::AABB GameObject::GetAABB() const
{
	return bounding_box;
}

void GameObject::OnSave(JSON_Object* obj)
{
	json_object_set_string(obj, "name", name);
	json_object_set_number(obj, "uid", uid);
	json_object_set_number(obj, "uid parent", parent->uid);

	JSON_Value* arr_components = json_value_init_array();
	JSON_Array* components_array = json_value_get_array(arr_components);

	for (uint i = 0; i < components.size(); ++i)
	{
		JSON_Value* newValue = json_value_init_object();
		JSON_Object* objToSerialize = json_value_get_object(newValue);

		components[i]->OnSave(objToSerialize);
		json_array_append_value(components_array, newValue);
	}

	json_object_set_value(obj, "Components", arr_components);
}

void GameObject::OnLoad(JSON_Object* obj)
{
	strcpy(name, json_object_get_string( obj, "name"));
	uid = json_object_get_number(obj, "uid");

	JSON_Array* components_array = json_object_get_array(obj, "Components");
	JSON_Object* obj_components;

	for (int i = 0; i < json_array_get_count(components_array); i++) {

		obj_components = json_array_get_object(components_array, i);
		Component* newComponent = AddComponent((ComponentType)(int)json_object_get_number(obj_components, "type"));
		newComponent->OnLoad(obj_components);
	}

}