#version 420
in vec3 vPosition;   
in vec4 vColor;
in vec3 vVelocity;

out vec4 color;
out float y;
uniform mat4 projection;
uniform mat4 model_view;
uniform float time;

void main(){
	float position_x=vPosition.x+0.001*vVelocity.x*time;
	float position_z=vPosition.z+0.001*vVelocity.z*time;
	float a=-0.00000049;
	float position_y=vPosition.y+0.001*vVelocity.y*time+0.5*a*time*time;
	y=position_y;
	vec4 position=vec4(position_x, position_y, position_z, 1.0);
	gl_Position =  projection * model_view * position;
	color=vColor;
}