#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	resources.clear();

}

//The file need to load in ModuleFileSystem
uint ResourceManager::Find(const char* file) const
{
	
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
		if (strcmp(it->second->GetExportedFile(), file) == 0)
			return it->first;

	return 0;
}

uint ResourceManager::ImportFile(const char* new_file)
{
	uint ret = 0;

	ret = Find(new_file);

	if (ret == 0)
	{
		LOG("Don't find in resources, need load from path...");

		FILE_TYPE file_type = App->fs->FindTypeFile(new_file);

		Resource* resource = nullptr;

		switch (file_type)
		{
		case MESH_FILE:
			LOG("Mesh file detected, creating...");
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
			
			break;
		case MATERIAL_FILE:
			LOG("Material file detected, creating...");
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);

			break;
		case UKNOWN_FILE:
			LOG("Can't recognize type of file");
			break;
		}

		if (resource != nullptr)
			ret = resource->GetUID();
		else
			ret = 0;
	}
	else
	{
		LOG("Find file in resources with uid: %i", ret);
	}

	return ret;
}

const Resource* ResourceManager::Get(uint uid)
{
	Resource* res = nullptr;

	std::map<uint, Resource*>::iterator it= resources.find(uid);

	if (it != resources.end())
		res = it->second;

	return res;
}

Resource* ResourceManager::CreateNewResource(RESOURCE_TYPE type)
{
	Resource* resource = nullptr;

	if (type != RESOURCE_TYPE::RESOURCE_DEFAULT)
	{
		uint uid = App->random->Int();

		switch (type)
		{
		case RESOURCE_MESH:
			resource = (Resource*) new ResourceMesh(uid, RESOURCE_MESH);
			break;
		case RESOURCE_MATERIAL:
			resource = (Resource*) new ResourceMaterial(uid, RESOURCE_MATERIAL);
			break;
		default:
			break;
		}

		if (resource != nullptr)
		{
			LOG("NEW RESOURCE GENERATED");
			resources[uid] = resource;
		}
		else
			LOG("Can't generate new resource :(");
	}

	return resource;
}