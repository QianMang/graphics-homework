#pragma once
#include "Light.h"
class PointLight :public Light
{
public:
	PointLight();
	PointLight(color4 aIntensity, color4 dIntensity, color4 sIntensity, vec3 pos, GLfloat constant, GLfloat linear, GLfloat quad);
	void SetPointLight(GLuint program, vec4 light_position_eyeFrame,int light);
	vec3 getPosition();
	void getInfo();
	~PointLight();


protected:
	vec3 position;
	GLfloat constant, linear, quad;
};

