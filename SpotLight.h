#pragma once
#include "PointLight.h"
typedef Angel::vec3  point3;
class SpotLight : public PointLight
{
public:
	SpotLight();
	SpotLight(color4 aIntensity, color4 dIntensity, color4 sIntensity,
		vec3 pos, GLfloat constant, GLfloat linear, GLfloat quad,
		vec3 direction, point3 at_position, GLfloat cutOff,GLfloat exponent );
	void setSpotLight(GLuint program);
	vec3 get_at_position();
	void set_mv_Direction(vec3 d);
	~SpotLight();

private:
	vec3 direction;
	point3 light_at_position;
	GLfloat cutOff;
	GLfloat exponent;
};

