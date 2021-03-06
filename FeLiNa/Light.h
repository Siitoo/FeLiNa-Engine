#ifndef _LIGHT_
#define _LIGHT

#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	math::float3 position;

	int ref;
	bool on;
};

#endif // !_LIGHT_