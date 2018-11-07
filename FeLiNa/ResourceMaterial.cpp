#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{
}

ResourceMaterial::~ResourceMaterial()
{
}

bool ResourceMaterial::LoadInMemory()
{
	bool ret = false;

	return ret;
}

bool ResourceMaterial::EraseInMemory()
{
	bool ret = false;

	return ret;
}

void ResourceMaterial::SetTexture(Texture* texture)
{
	this->texture = *texture;
}

Texture ResourceMaterial::GetTexture() const
{
	return texture;
}