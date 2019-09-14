#include "Light.h"



Light::Light()
{
	
}

Light::Light(color4 aIntensity, color4 dIntensity, color4 sIntensity)
{
	ambientColor = aIntensity;
	diffuseColor = dIntensity;
	specularColor = sIntensity;
}

void Light::setProduct(vec4 global_a,vec4 m_a, vec4 m_d, vec4 m_s)  //material
{
	globalAmbientProduct = global_a * m_a;
	ambientProduct = ambientColor * m_a;
	diffuseProduct = diffuseColor * m_d;
	specularProduct = specularColor * m_s;
}

void Light::UseLightProduct(GLuint program,int light)  //light=1: directional,    light=2: point,   light=3: spot
{
	if (light == 1) {
		glUniform4fv(glGetUniformLocation(program, "directionalLight.base.GlobalAmbientProduct"),
			1, globalAmbientProduct);
		glUniform4fv(glGetUniformLocation(program, "directionalLight.base.AmbientProduct"),
			1, ambientProduct);
		glUniform4fv(glGetUniformLocation(program, "directionalLight.base.DiffuseProduct"),
			1, diffuseProduct);
		glUniform4fv(glGetUniformLocation(program, "directionalLight.base.SpecularProduct"),
			1, specularProduct);
	}
	else if (light == 2) {
		glUniform4fv(glGetUniformLocation(program, "pointLight.base.GlobalAmbientProduct"),
			1, globalAmbientProduct);
		glUniform4fv(glGetUniformLocation(program, "pointLight.base.AmbientProduct"),
			1, ambientProduct);
		glUniform4fv(glGetUniformLocation(program, "pointLight.base.DiffuseProduct"),
			1, diffuseProduct);
		glUniform4fv(glGetUniformLocation(program, "pointLight.base.SpecularProduct"),
			1, specularProduct);

	}
	else if (light == 3) {
		glUniform4fv(glGetUniformLocation(program, "spotLight.base.base.GlobalAmbientProduct"),
			1, globalAmbientProduct);
		glUniform4fv(glGetUniformLocation(program, "spotLight.base.base.AmbientProduct"),
			1, ambientProduct);
		glUniform4fv(glGetUniformLocation(program, "spotLight.base.base.DiffuseProduct"),
			1, diffuseProduct);
		glUniform4fv(glGetUniformLocation(program, "spotLight.base.base.SpecularProduct"),
			1, specularProduct);
	}
}




Light::~Light()
{
}
