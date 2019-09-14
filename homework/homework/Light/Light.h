#pragma once
#include "../Angel-yjc.h"

typedef Angel::vec3  color3;
typedef Angel::vec4  color4;
class Light
{
public:
	Light();
	Light( color4 aIntensity, color4 dIntensity,color4 sIntensity);
	void setProduct(vec4 global_ambient,vec4 m_ambient, vec4 m_diffuse, vec4 m_specular);
	void UseLightProduct(GLuint program,int light);
	~Light();


protected:
	color4 ambientColor, diffuseColor, specularColor;
	vec4 globalAmbientProduct;
	vec4 ambientProduct;
	vec4 diffuseProduct;
	vec4 specularProduct;
};

