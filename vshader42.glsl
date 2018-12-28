/***************************
 * File: vshader42.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/


#version 420
in  vec3 vPosition;
in  vec4 vColor;
in  vec3 normal;    //face normal
in  vec3 vNormal;   // vertex normal
in  vec2 vTexCoord;  
 
out vec4 color;
out vec3 FragPos;
out vec2 texCoord_2D;
out float texCoord_1D;
out vec2 texCoord_lattice;

uniform mat4 model_view;

uniform mat4 projection;

uniform mat3 Normal_Matrix;

uniform float Shininess;  
uniform int light;     //enable light
uniform int shading;   // 0: flat shading  1: smooth shading

uniform int pointLightCount;
uniform int spotLightCount;

struct Light{
	vec3 colour;
	vec4 GlobalAmbientProduct,AmbientProduct, DiffuseProduct, SpecularProduct;
};
struct DirectionalLight{
	Light base;
	vec3 direction;
};
struct PointLight{
	Light base;
	vec4 lightPosition;
	float constant,linear,quad;
};
struct SpotLight{
	PointLight base;
	vec3 direction;
	float cutOff;
	float exponent;
};

struct Texture{
	int textureFlag;
	int texture_way;  // 1:vertical  2: slanted   
	int texture_space;   //1: object  2: eye
	int texture_object;  //0: nothing 1: sphere   2:ground   3: shadow
	bool texture_lattice;
	int texture_lattice_pattern;    //1: upright  2: tilted
};

uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform Texture _texture;

vec3 curNormal;
float s;   //for the sphere texCoord computation  1D
float s2;   //2d _texture
float t2;
float s3;
float t3;
void CalcTexcood_1D(){
	if(_texture.texture_space==1){
		if(_texture.texture_way==1){
			s=2.5* vPosition.x;
		}else if(_texture.texture_way==2){
			s=1.5*(vPosition.x+vPosition.y+vPosition.z);
		}
	}else if(_texture.texture_space==2){
		vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
		if(_texture.texture_way==1){
			s=2.5* ( model_view * vPosition4).x;
		}else if(_texture.texture_way==2){
			s=1.5*(( model_view * vPosition4).x+( model_view * vPosition4).y+( model_view * vPosition4).z);
		}
	}
}

void CalcTexcood_2D(){
	if(_texture.texture_space==1){
		if(_texture.texture_way==1){
			s2=0.5* (vPosition.x+1);
			t2=0.5*(vPosition.y+1);
		}else if(_texture.texture_way==2){
			s2=0.3*(vPosition.x+vPosition.y+vPosition.z);
			t2=0.3*(vPosition.x-vPosition.y+vPosition.z);
		}
	}else if(_texture.texture_space==2){
		vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
		if(_texture.texture_way==1){
			s2=0.5* (( model_view * vPosition4).x+1);
			t2=0.5* (( model_view * vPosition4).y+1);
		}else if(_texture.texture_way==2){
			s2=0.3*(( model_view * vPosition4).x+( model_view * vPosition4).y+( model_view * vPosition4).z);
			t2=0.3*(( model_view * vPosition4).x-( model_view * vPosition4).y+( model_view * vPosition4).z);
		}
	}

}

void CalcLattice(){
	if(_texture.texture_lattice_pattern==1){   //upright
		s3=0.5*(vPosition.x+1);
		t3=0.5* (vPosition.y+1);
	}else if(_texture.texture_lattice_pattern==2){
		s3=0.3*(vPosition.x+vPosition.y+vPosition.z);
		t3=0.3*(vPosition.x-vPosition.y+vPosition.z);
	
	}
}

vec4 CalcLightByDirection(Light light,vec3 direction){
	vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
	vec3 pos =(model_view * vPosition4).xyz;
	
	vec3 L= normalize(-direction);
	vec3 V= normalize(-pos);
	vec3 H= normalize(L+V);
	vec3 N= normalize(Normal_Matrix * curNormal);
	if(dot(N,V)<0) N=-N;
	vec4 ambient = light.AmbientProduct;
		
	float d=max(dot(L,N),0.0);
	vec4 diffuse=d*light.DiffuseProduct;
		
	float s=pow(max(dot(N,H),0.0),Shininess);
	vec4 specular=s*light.SpecularProduct;
	if(dot(L,N)<0.0){
		specular=vec4(0.0,0.0,0.0,1.0);
	}
		
	
	return (ambient+diffuse+specular);

}

void main() 
{
	if(_texture.texture_object==1)   // object is sphere
	{	
		CalcTexcood_1D();
		CalcTexcood_2D();
		
	}
	if(_texture.texture_object==1 ||_texture.texture_object==3){
		if(_texture.texture_lattice==true){
			CalcLattice();
			texCoord_lattice=vec2(s3,t3);
		}
	}
	vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
	if(light==1){
		if(shading==0)
			curNormal=normalize(normal);
		else
			curNormal=normalize(vNormal);
		color=CalcLightByDirection(directionalLight.base,directionalLight.direction);
		vec4 pos = model_view*vPosition4;
		
		if(pointLightCount!=0){
			vec3 point_direction=(pos-pointLight.lightPosition).xyz;
			float distance=length(point_direction);
			vec4 point_color=CalcLightByDirection(pointLight.base,point_direction);
			float attenuation=pointLight.quad*distance*distance+
								pointLight.linear*distance+
								pointLight.constant;
			attenuation=1/attenuation;
			color=color+point_color*attenuation;
		}
		
		if(spotLightCount!=0){
			vec3 point_direction=(pos-spotLight.base.lightPosition).xyz;
			float theta=dot(normalize(point_direction),normalize(spotLight.direction));
			
			//color=vec4(theta,theta,theta,1);
			if(theta > spotLight.cutOff){
				float distance=length(point_direction);
				vec4 spot_color=CalcLightByDirection(spotLight.base.base,point_direction);
				float intensity=pow(theta,spotLight.exponent);
				float attenuation=spotLight.base.quad*distance*distance+
								spotLight.base.linear*distance+
								spotLight.base.constant;
				attenuation=intensity*(1/attenuation);
				color=color+spot_color*attenuation;
			
			}
		
		}
		color=color+directionalLight.base.GlobalAmbientProduct;
	}
    else{
		color = vColor;
	}
	gl_Position =  projection * model_view * vPosition4;
	FragPos=(model_view * vPosition4).xyz;
	if(_texture.texture_object==2)      //2d _texture for ground
		texCoord_2D=vTexCoord;
	else if(_texture.texture_object==1)   //2d _texture for sphere
		texCoord_2D=vec2(s2,t2);
	texCoord_1D=s;
	
} 
