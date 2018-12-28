#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(color4 aIntensity, color4 dIntensity, color4 sIntensity,vec3 d);

	void SetDirectionalLight(GLuint program);
	~DirectionalLight();

private:
	vec3 direction;

};

