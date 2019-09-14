#version 420

in vec4 color;
in float y;
out vec4 fColor;

void main(){
	if(y<0.1){
		discard;
	}
	fColor=color;
}