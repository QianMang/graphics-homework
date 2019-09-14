#include "SpotLight.h"



SpotLight::SpotLight()
{
}

SpotLight::SpotLight(color4 aIntensity, color4 dIntensity, color4 sIntensity, 
	vec3 pos, GLfloat constant, GLfloat linear, GLfloat quad, 
	vec3 _direction, point3 at_position, GLfloat _cutOff, GLfloat _exponent):PointLight(aIntensity, dIntensity, sIntensity, pos,  constant,  linear,  quad)
{
	direction = _direction;
	light_at_position = at_position;
	cutOff = _cutOff;
	exponent = _exponent;
	//std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
}

void SpotLight::setSpotLight(GLuint program)
{
	//std::cout << newDirection.x << " " << newDirection.y << " " << newDirection.z << std::endl;
	glUniform3fv(glGetUniformLocation(program, "spotLight.direction"),
		1, direction);
	glUniform1f(glGetUniformLocation(program, "spotLight.cutOff"),
		cos(DegreesToRadians*cutOff));
	glUniform1f(glGetUniformLocation(program, "spotLight.exponent"),
		exponent);
	
}

vec3 SpotLight::get_at_position()
{
	
	return light_at_position;
}

void SpotLight::set_mv_Direction(vec3 _newDirection)
{
	direction.x = _newDirection.x;
	direction.y = _newDirection.y;
	direction.z = _newDirection.z;
}


SpotLight::~SpotLight()
{
}
