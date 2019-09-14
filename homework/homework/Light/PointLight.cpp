#include "PointLight.h"



PointLight::PointLight()
{
}

PointLight::PointLight(color4 aIntensity, color4 dIntensity, color4 sIntensity,
									vec3 pos, GLfloat con, GLfloat lin, GLfloat qua):Light(aIntensity, dIntensity, sIntensity)
{
	position = pos;
	constant = con;
	linear = lin;
	quad = qua;
}

void PointLight::SetPointLight(GLuint program,vec4 light_position_eyeFrame,int light)
{
	if (light == 2) {
		glUniform4fv(glGetUniformLocation(program, "pointLight.lightPosition"),
			1, light_position_eyeFrame);
		glUniform1f(glGetUniformLocation(program, "pointLight.constant"),
			constant);
		glUniform1f(glGetUniformLocation(program, "pointLight.linear"),
			linear);
		glUniform1f(glGetUniformLocation(program, "pointLight.quad"),
			quad);
	}
	else if (light == 3) {
		glUniform4fv(glGetUniformLocation(program, "spotLight.base.lightPosition"),
			1, light_position_eyeFrame);
		glUniform1f(glGetUniformLocation(program, "spotLight.base.constant"),
			constant);
		glUniform1f(glGetUniformLocation(program, "spotLight.base.linear"),
			linear);
		glUniform1f(glGetUniformLocation(program, "spotLight.base.quad"),
			quad);
	}
}
vec3 PointLight::getPosition() {
	//std::cout << position.x << " " << position.y << " " <<position.z << std::endl;
	return  position;
}

void PointLight::getInfo()
{
	std::cout << this->ambientProduct.x <<" "<<this->ambientProduct.y<< std::endl;
}

PointLight::~PointLight()
{
}
