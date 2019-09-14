#include "DirectionalLight.h"



DirectionalLight::DirectionalLight()
{
	direction = vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(color4 aIntensity, color4 dIntensity, color4 sIntensity,vec3 d):Light( aIntensity,dIntensity,sIntensity)
{
	
	direction = d;
}




void DirectionalLight::SetDirectionalLight(GLuint program)
{
	glUniform3fv(glGetUniformLocation(program, "directionalLight.direction"),
		1, direction);
}

DirectionalLight::~DirectionalLight()
{
}
