/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

// #version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version
#version 440
in vec4 color;
in vec3 FragPos;
in vec2 texCoord_2D;
in float texCoord_1D;
in vec2 texCoord_lattice;
out vec4 fColor;

struct Fog{
	int fogIndex;   //0,1,2,3
	vec4 fogColor;
	float fogStart;
	float fogEnd;
	float fogDensity;

};
struct Texture{
	int textureFlag;
	int texture_way;  // 1:vertical  2: slanted   
	int texture_space;   //1: object  2: eye
	int texture_object;  //0: nothing 1: sphere   2:ground   3: shadow
	bool texture_lattice;
	int texture_lattice_pattern;    //1: upright  2: tilted
};


uniform Fog fog;
uniform sampler2D texture_2D;
uniform sampler1D texture_1D;

uniform Texture _texture;

void Lattice(){
	if(fract(4*texCoord_lattice.x)<0.35 && fract(4*texCoord_lattice.y)<0.35)
		discard;

}
void main() 
{ 
	if((_texture.texture_object==1 || _texture.texture_object==3) && _texture.texture_lattice==true)
		Lattice();
	float distance=length(FragPos);
	float fog_factor;
	if(fog.fogIndex==0){
		fog_factor=1;
	}else if(fog.fogIndex==1){
		clamp(distance,fog.fogStart,fog.fogEnd);
		fog_factor=(fog.fogEnd-distance)/(fog.fogEnd-fog.fogStart);
	}else if(fog.fogIndex==2){
		fog_factor=exp(-fog.fogDensity*distance);
	}else if(fog.fogIndex==3){
		fog_factor=exp(-pow(fog.fogDensity*distance,2));
	}
	clamp(fog_factor,0,1);
	if(_texture.textureFlag==1)     //1d texture
		fColor=mix(fog.fogColor,color*texture(texture_1D,texCoord_1D) ,fog_factor) ;
	else if(_texture.textureFlag==2){   //2d texture
	    vec4 textureColor=texture(texture_2D,texCoord_2D);
		if(_texture.texture_object==1){
			if(textureColor.x<1.0)
				textureColor=vec4(0.9,0.1,0.1,1.0);
		}
	    fColor=mix(fog.fogColor,color*textureColor,fog_factor);
	}
	else
		fColor=mix(fog.fogColor,color,fog_factor);
} 

