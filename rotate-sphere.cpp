/************************************************************
Fangsheng Dong 
ID: fd820

include Angle-yjc.h, CheckError.h, mat-yjc-new.h, vec.h,InitShader.cpp, fshader42.glsl, vshader42.glsl    
same as the last sample project.
And also include ------ "Light.h" "DirectionalLight.h" "PointLight.h" "SpotLight.h" "Texture.h" and their cpp file
put the vertex txt file in the same directory.

press 'b' to begin
**************************************************************/
#include "Angel-yjc.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include"Texture.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint program_particles;
//GLuint cube_buffer;   /* vertex buffer object id for cube */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint axis_buffer;
GLuint shadow_buffer;
GLuint particles_buffer;
//
GLuint sphere_buffer;                        
// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 15.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0,3.0,-10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by key ''B,b"
int sphereFlag = 1;   // 1: solid sphere; 0: wireframe sphere. 
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. 
int startFlag = 0; //1: started
int shadowFlag = 1;   //produce shadow
int lightFlag = 1;   //enable light
int shadingFlag = 0;   //0:flat shading,  1: smooth shading
int fogFlag = 0; // 0: no fog ,1: linear ,2: exponential 3: exponential square.
int blendFlag = 1;    
int textureGroundFlag = 1;
int textureSphereFlag = 1;  //0: sphere no texture   1:Contour Lines   2: Checkerboard

int particlesFlag = 0;
///////
int vertices_N = 0;
int triangle_N = 0;
point3 sphere_points[5000];
color4 sphere_colors[5000];
color4 shadow_colors[5000];
point3 axis_points[6];
color4 axis_colors[6];
int sphere_NumVertices = 0;
int axis_NumVertices = 6;           //
int normal_NumVertices = 0;
const int particles_N = 300;       //the num of particles
point3 particles_points[particles_N];
color4 particles_colors[particles_N];
vec3 particles_velocity[particles_N];    //v0
vec3   normals[5000];
vec3 vertex_normals[5000];
point3 A(3.0, 1.0, 5.0);
//point3 A(-7.0, 1.0, 10.0);
point3 B(-2.0, 1.0, -2.5);
point3 C(2.0,1.0,-4.0);
int phase = 0; // 0: A ,1:B ,2:C
GLfloat distance_AB =length(B - A);
GLfloat distance_BC = length(C - B);
GLfloat distance_CA = length(A - C);

mat4 rotate;
mat4 translation;
mat4 accumulate_Rotate = { 1.0 };
mat4 N(12.0,0.0,0.0,0.0,           //shadow projection  matrix
				14.0,0.0,3.0,-1.0,
				0.0,0.0,12.0,0.0,
				0.0,0.0,0.0,12.0 );
#if 0
mat4 Otho_Pro(1, 0, 0, 0,     //test
						0, 0, 0, 0,
						0, 0, 1, 0,
						0, 0,0, 1);
#endif
const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floor_normals[floor_NumVertices];
vec2 floor_texCoord[floor_NumVertices] = {
	vec2(10.0 / 8.0,12.0 / 8.0),vec2(0.0,0.0),vec2(0.0,12.0/8.0),vec2(0.0,0.0),vec2(10.0 / 8.0,12.0 / 8.0),vec2(10.0 / 8.0,0.0),
};
// Vertices of a unit cube centered at origin, sides aligned with axes
point3 vertices[8] = {
    point3( 5.0, 0.0,  8.0),
    point3( 5.0, 0.0,  -4.0),
	point3(-5.0, 0.0,  -4.0),
    point3( -5.0, 0.0,  8.0),
    point3(0.0,0.02,0.0),
	point3(0.0,10.0,0.0),//y
	point3(10.0,0.02,0.0),//x
    point3(0.0,0.02,10.0)//z
};
// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0,1.0),  // black
    color4( 1.0, 0.0, 0.0,1.0),  // red
    color4( 1.0, 1.0, 0.0,1.0),  // yellow
    color4( 0.0, 1.0, 0.0,1.0),  // green
    color4( 0.0, 0.0, 1.0,1.0),  // blue
    color4( 1.0, 0.0, 1.0,1.0),  // magenta
    color4( 1.0, 1.0, 1.0,1.0),  // white
    color4( 0.0, 1.0, 1.0,1.0)   // cyan
};

class Face {
public:
	Face(point3 a, point3 b, point3 c, vec3 n) {
		p1 = a; p2 = b; p3 = c; normal = n;
	}
	point3 p1;
	point3 p2;
	point3 p3;
	vec3 normal;
	bool contain(point3 p) {
		if ((p.x == p1.x && p.y==p1.y && p.z==p1.z )|| (p.x == p2.x && p.y == p2.y && p.z == p2.z) || (p.x == p3.x && p.y == p3.y && p.z == p3.z))
			return true;
		else
			return false;
	}
};

/*----- Shader Lighting Parameters -----*/
color4 global_ambient(1.0, 1.0, 1.0, 1.0);

color4 directionalLight_ambient(0.0, 0.0, 0.0, 1.0);
color4 directionalLight_diffuse(0.8, 0.8, 0.8, 1.0);
color4 directionalLight_specular(0.2, 0.2, 0.2, 1.0);
vec3 light_direction(0.1, 0.0, -1.0);
DirectionalLight directionalLight = DirectionalLight(directionalLight_ambient, directionalLight_diffuse, directionalLight_specular,light_direction);
color4 pointLight_ambient(0.0, 0.0, 0.0, 1.0);          //same as spot light
color4 pointLight_diffuse(1.0, 1.0, 1.0, 1.0);
color4 pointLight_specular(1.0, 1.0, 1.0, 1.0);
point3 light_position(-14.0, 12.0, -3.0);        //point light or spot light
GLfloat const_att = 2.0;
GLfloat linear_att = 0.01;
GLfloat quad_att = 0.001;
vec3 spotLight_direction(8.0, -12.0, -1.5);
point3 light_at_position(-6.0, 0.0, -4.5);
GLfloat exponent = 15;
GLfloat cutOff = 20.0;
PointLight pointLight = PointLight(pointLight_ambient, pointLight_diffuse, pointLight_specular, light_position, const_att, linear_att, quad_att);
SpotLight spotLight = SpotLight(pointLight_ambient, pointLight_diffuse, pointLight_specular, light_position, const_att, linear_att, quad_att, spotLight_direction,light_at_position,cutOff, exponent);

// In World frame.
// Needs to transform it to Eye Frame
// before sending it to the shader(s).
color4 material_ambient;
color4 material_diffuse;
color4 material_specular;
float  material_shininess = 125.0;

color4 sphere_ambient(0.2, 0.2, 0.2, 1.0);
color4 sphere_diffuse(1.0, 0.84, 0.0, 1.0);
color4 sphere_specular(1.0, 0.84, 0.0, 1.0);
color4 floor_ambient(0.2, 0.2, 0.2, 1.0);
color4 floor_diffuse(0.0,1.0,0.0,1.0);
color4 floor_specular(0.0, 0.0, 0.0, 1.0);

color4 ambient_product;
color4 diffuse_product;
color4 specular_product;

int pointLightCount = 0;
int spotLightCount=0;

color4 fogColor(0.7, 0.7, 0.7, 0.5);
GLfloat fog_start = 0.0;
GLfloat fog_end = 18.0;
GLfloat fog_density = 0.09;
//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors

float time = 0;
float max_time = 10000;
float cur_time=0;
float last_time = 0;
float sub_time = 0;

Texture sphereTexture=Texture(1);
Texture floorTexture=Texture(2);
Texture axisTexture=Texture(0);
Texture shadowTexture=Texture(3);

GLfloat stringToNum(std::string& str)
{
	std::istringstream iss(str);
	GLfloat num;
	iss >> num;
	return num;
}

void caculateVertexNormal(std::vector<Face> allFace) {      //another caculation way
	for (int i = 0; i < Index; i++) {
		vec3 vertexNormal(0, 0, 0);
		for (int j = 0; j < allFace.size(); j++) {
			if (allFace[j].contain(sphere_points[i])) {
				//std::cout << "1" << std::endl;
				vertexNormal = vertexNormal + allFace[j].normal;
			}
		}
		vertex_normals[i] = normalize(vertexNormal);
	}
}

void readFile() {
	std::string fileName;
	std::fstream f;
	
	while (1) {
		std::cout << std::endl;
		std::cout << "-------------------------------------" << std::endl;
		std::cout << "input file name:(such as xxx.txt)" << std::endl;
		std::cin >> fileName;
		f = std::fstream(fileName);
		if (!f.is_open()) {
			std::cout << "file does not exist" << std::endl;
			continue;
		}
		break;
	}
	std::string line;
	getline(f, line);
	triangle_N = stoi(line);
	sphere_NumVertices = triangle_N * 3;
	normal_NumVertices = sphere_NumVertices;
	int n;
	int v_i=0;
	point3 vertices;
	std::vector<Face> AllFace;
	for (int i = 0; i < triangle_N; i++) {
		getline(f, line);
		n = stoi(line);
		for (int j = 0; j < n; j++) {
			getline(f, line);
			std::string vertex_i = "";
			for (unsigned int k = 0; k < line.length(); k++) {
				if (line[k] == ' ') {
					vertices[v_i] = stringToNum(vertex_i);
					v_i++;
					vertex_i = "";	
				}
				else if (k == line.length() - 1) {
					vertex_i = vertex_i + line[k];
					vertices[v_i] = stringToNum(vertex_i);
					v_i++;
					vertex_i = "";
				}
				vertex_i = vertex_i + line[k];
			}
			sphere_points[Index] = vertices;
			sphere_colors[Index] = color4(1.0, 0.84, 0,1.0);
			shadow_colors[Index] = color4(0.25, 0.25, 0.25, 0.65);

			Index++;
			v_i = 0;
		}
		vec3 u = normalize(sphere_points[Index-1] - sphere_points[Index - 2]);
		vec3 v = normalize(sphere_points[Index - 3] - sphere_points[Index - 2]);
		vec3 normal = normalize(cross(u, v));
		for (int k = 1; k <= 3; k++) {
			normals[Index - k] = normal;
			vertex_normals[Index - k] = sphere_points[Index - k];
	
		}
		//Face cur_face = Face(sphere_points[Index - 1], sphere_points[Index - 2], sphere_points[Index - 3], normal);
		//AllFace.push_back(cur_face);
		

	}
	//caculateVertexNormal(AllFace);
}



void floor()
{
	vec3 up(0, 1, 0);
	floor_colors[0] = vertex_colors[3]; floor_points[0] = vertices[2]; floor_normals[0] = up;
    floor_colors[1] = vertex_colors[3]; floor_points[1] = vertices[0]; floor_normals[1] = up;
    floor_colors[2] = vertex_colors[3]; floor_points[2] = vertices[1]; floor_normals[2] = up;

    floor_colors[3] = vertex_colors[3]; floor_points[3] = vertices[0]; floor_normals[3] = up;
    floor_colors[4] = vertex_colors[3]; floor_points[4] = vertices[2]; floor_normals[4] = up;
    floor_colors[5] = vertex_colors[3]; floor_points[5] = vertices[3]; floor_normals[5] = up;
	
}



void axis() {
	axis_colors[0] = vertex_colors[5]; axis_points[0] = vertices[4];
	axis_colors[1] = vertex_colors[5]; axis_points[1] = vertices[5];
	
	axis_colors[2] = vertex_colors[1]; axis_points[2] = vertices[6];
	axis_colors[3] = vertex_colors[1]; axis_points[3] = vertices[4];
	
	axis_colors[4] = vertex_colors[4]; axis_points[4] = vertices[4];
	axis_colors[5] = vertex_colors[4]; axis_points[5] = vertices[7];
	
}

void particles() {
	for (int i = 0; i < particles_N; i++) {
		particles_points[i] = vec3(0.0, 0.1, 0.0);
		GLfloat v_x = 2.0*((rand() % 256) / 256.0 - 0.5);
		//std::cout << v_x << std::endl;                                         //
		GLfloat v_z = 2.0*((rand() % 256) / 256.0 - 0.5);
		GLfloat v_y = 1.2*2.0*((rand() % 256) / 256.0);
		particles_velocity[i] = vec3(v_x, v_y, v_z);
		//std::cout << particles_velocity[i].x << std::endl;
		GLfloat r = (rand() % 256) / 256.0;
		GLfloat g = (rand() % 256) / 256.0;
		GLfloat b = (rand() % 256) / 256.0;
		particles_colors[i] = color4(r, g, b, 1.0);
	}
}

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
	sphereTexture.InitTexture();
	floorTexture.InitTexture();
	readFile();

 // Create and initialize a vertex buffer object for sphere, to be used in display()
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
#if 1
    glBufferData(GL_ARRAY_BUFFER, 
         sizeof(point3)*sphere_NumVertices + sizeof(color4)*sphere_NumVertices+sizeof(vec3)*normal_NumVertices+sizeof(vec3)*normal_NumVertices,
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
          sizeof(point3) * sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, 
          sizeof(point3) * sphere_NumVertices, 
          sizeof(color4) * sphere_NumVertices,
          sphere_colors);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices,
		sizeof(vec3)*normal_NumVertices,
		normals);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices+ sizeof(vec3)*normal_NumVertices,
		sizeof(vec3)*normal_NumVertices,
		vertex_normals);
#endif
	//shadow_ buffer
	glGenBuffers(1, &shadow_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(point3)*sphere_NumVertices + sizeof(color4)*sphere_NumVertices+sizeof(vec3)*normal_NumVertices+sizeof(vec3)*normal_NumVertices,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		sizeof(point3) * sphere_NumVertices, sphere_points);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(point3) * sphere_NumVertices,
		sizeof(color4) * sphere_NumVertices,
		shadow_colors);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices,
		sizeof(vec3)*normal_NumVertices,
		normals);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices + sizeof(vec3)*normal_NumVertices,
		sizeof(vec3)*normal_NumVertices,
		vertex_normals);

    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors)+ sizeof(floor_normals)+sizeof(floor_normals)+sizeof(floor_texCoord),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), 
					floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points)+sizeof(floor_colors), sizeof(floor_normals),
					floor_normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors)+sizeof(floor_normals), sizeof(floor_normals),
					floor_normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals) + sizeof(floor_normals), sizeof(floor_texCoord),
					floor_texCoord);

	// Create and initialize a vertex buffer object for axis, to be used in display()
	axis();          //
	glGenBuffers(1, &axis_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_points) + sizeof(axis_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axis_points), axis_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(axis_points), sizeof(axis_colors),axis_colors);

	//Create and initialize a vertex buffer object for particles, to be used in display()
	particles();
	glGenBuffers(1, &particles_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles_points) + sizeof(particles_colors) + sizeof(particles_velocity),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particles_points), particles_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(particles_points), sizeof(particles_colors), particles_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(particles_points)+sizeof(particles_colors), sizeof(particles_velocity), particles_velocity);


 // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader42.glsl", "fshader42.glsl");
	program_particles=InitShader("vshader_particles.glsl", "fshader_particles.glsl");
    
    glEnable( GL_DEPTH_TEST );
    glClearColor(0.529, 0.807, 0.92, 0.0);
	//glClearColor(0.7,0.7,0.7,0.5);
	//glClearDepth(1);
    glLineWidth(1.0);
	glPointSize(3.0);
}

void SetUp_Material(color4 ambient,color4 diffuse,color4 specular,float shininess) {
	material_ambient = ambient;
	material_diffuse=diffuse;
	material_specular=specular;
	material_shininess = shininess;
	directionalLight.setProduct(global_ambient, material_ambient, material_diffuse, material_specular);
	pointLight.setProduct(global_ambient, material_ambient, material_diffuse, material_specular);
	spotLight.setProduct(global_ambient, material_ambient, material_diffuse, material_specular);
}


void SetUp_Lighting_Uniform_Vars(mat4 mv)
{
	
	directionalLight.UseLightProduct(program,1);
	directionalLight.SetDirectionalLight(program);
	
	if (pointLightCount != 0) {																									////////////////////////////////
		vec4 light_position_eyeFrame = mv * pointLight.getPosition();
		
		pointLight.UseLightProduct(program,2);
		pointLight.SetPointLight(program, light_position_eyeFrame,2);
		
	}
	if (spotLightCount != 0) {
		vec4 light_position_eyeFrame = mv * spotLight.getPosition();
		vec4 spotLight_direction_eyeFrame = mv * spotLight.get_at_position()- light_position_eyeFrame;
		vec3 newDirection(spotLight_direction_eyeFrame.x, spotLight_direction_eyeFrame.y, spotLight_direction_eyeFrame.z);
		spotLight.set_mv_Direction(newDirection);
		spotLight.UseLightProduct(program, 3);
		spotLight.SetPointLight(program, light_position_eyeFrame, 3);
		spotLight.setSpotLight(program);
	}
	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);
}

//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//use for GL_TRIANGLES
void drawObj(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );
	
    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor,4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices));

	GLuint normal = glGetAttribLocation(program, "normal");
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices+ sizeof(color4) * num_vertices+sizeof(vec3)*num_vertices));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices + sizeof(vec3)*num_vertices+sizeof(vec3)*num_vertices));
      // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	
    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(vNormal);
	glDisableVertexAttribArray(vTexCoord);
}

//use for GL_LINES
void drawObj2(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor,4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));

	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_LINES, 0, num_vertices);
	
	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}

void drawPoints(GLuint buffer, int num_vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	GLuint vPosition = glGetAttribLocation(program_particles, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program_particles, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));

	GLuint vVelocity = glGetAttribLocation(program_particles, "vVelocity");
	glEnableVertexAttribArray(vVelocity);
	glVertexAttribPointer(vVelocity, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices+ sizeof(color4) * num_vertices));

	glDrawArrays(GL_POINTS, 0, num_vertices);

	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(vVelocity);
}

//----------------------------------------------------------------------------
void display( void )
{
	  GLuint  model_view;  // model-view matrix uniform shader variable location
	  GLuint  projection;  // projection matrix uniform shader variable location
	  GLuint view;
	 //GLuint shadow;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program
	glUniform1i(glGetUniformLocation(program, "spotLightCount"), spotLightCount);
	glUniform1i(glGetUniformLocation(program, "pointLightCount"), pointLightCount);

	glUniform1i(glGetUniformLocation(program, "fog.fogIndex"), fogFlag);                                     //set fog
	glUniform4fv(glGetUniformLocation(program, "fog.fogColor"),1, fogColor);
	glUniform1f(glGetUniformLocation(program, "fog.fogStart"), fog_start);
	glUniform1f(glGetUniformLocation(program, "fog.fogEnd"), fog_end);
	glUniform1f(glGetUniformLocation(program, "fog.fogDensity"), fog_density);

    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );
	view = glGetUniformLocation(program, "view");
	


	//shadow = glGetUniformLocation(program, "shadow");
/*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
	
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

	
/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(0.0,0.0,0.0, 0.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);
    mat4  mv = LookAt(eye, at, up);
	
	mat4 mv2;
	vec3 direction_v;
	vec3 start_v;
	GLfloat distance = angle / 360 * 2 * M_PI * 1;
	if (phase == 0) {
		if (distance < distance_AB) {
			direction_v = B - A;
			start_v = A - (0.0, 0.0, 0.0);
		}
		else{
			phase = 1;
			angle = 0;
			distance = 0;
			accumulate_Rotate = rotate * accumulate_Rotate;
			direction_v = C - B;
			start_v = B - (0.0, 0.0, 0.0);
		}

	}
	else if (phase == 1) {
		if (distance < distance_BC) {
			direction_v = C - B;
			start_v = B - (0.0, 0.0, 0.0);
			
		}
		else {
			phase = 2;
			accumulate_Rotate = rotate * accumulate_Rotate;
			angle = 0;
			distance = 0;
			direction_v = A - C;
			start_v = C - (0.0, 0.0, 0.0);
		}
		
	}
	else if (phase == 2) {
		if (distance < distance_CA) {
			direction_v = A - C;
			start_v = C - (0.0, 0.0, 0.0);
		}
		else {
			phase = 0;
			angle = 0;
			distance = 0;
			accumulate_Rotate = rotate * accumulate_Rotate;
			direction_v = B - A;
			start_v = A - (0.0, 0.0, 0.0);
		}
		
	}

	//caculate the rotation matrix
	
	vec3 rotate_v = cross(up, direction_v);
	rotate = Rotate(angle, rotate_v.x, rotate_v.y, rotate_v.z);

	//caculate the translation matrix
	
	vec3 translation_v;
	translation_v = start_v + distance *  normalize(direction_v);
	translation = Translate(translation_v);
/*----- Set Up the Model-View matrix for the sphere-----*/
	
	if (sphereFlag == 0) 
		glUniform1i(glGetUniformLocation(program, "light"), 0);
	else
		glUniform1i(glGetUniformLocation(program, "light"), lightFlag);
	glUniform1i(glGetUniformLocation(program, "shading"), shadingFlag);
	SetUp_Material(sphere_ambient, sphere_diffuse, sphere_specular, material_shininess);
	SetUp_Lighting_Uniform_Vars(mv);
	mv = mv * translation* rotate *accumulate_Rotate;            ///
	
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
	
    if (sphereFlag == 1) // Filled sphere
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe sphere
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	mat3 normal_matrix = NormalMatrix(mv, 1);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
		1, GL_TRUE, normal_matrix);
	if (textureSphereFlag == 1)
		sphereTexture.UseTexture(program, 1);
	else if (textureSphereFlag == 0)
		sphereTexture.UseTexture(program, 0);
	else if (textureSphereFlag == 2)
		sphereTexture.UseTexture(program, 2);
    drawObj(sphere_buffer, sphere_NumVertices);  // draw the sphere

												 
/*----- Set up the Mode-View matrix for the floor -----*/

	mv2 = LookAt(eye, at, up);
	glUniform1i(glGetUniformLocation(program, "light"), lightFlag);
	glUniform1i(glGetUniformLocation(program, "shading"), 0);
	SetUp_Material(floor_ambient, floor_diffuse, floor_specular, material_shininess);
	SetUp_Lighting_Uniform_Vars(mv2);
	mv2 =mv2* Translate(0.0, 0.0, 0.0);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2); // GL_TRUE: matrix is row-major
													//glUniformMatrix4fv(shadow, 1, GL_TRUE, s);
	if (floorFlag == 1) // Filled floor
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe floor
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	normal_matrix = NormalMatrix(mv2, 1);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
		1, GL_TRUE, normal_matrix);

	if(textureGroundFlag==1)                   //ground texture 
		floorTexture.UseTexture(program, 2);
	else
		floorTexture.UseTexture(program, 0);
	if(eye.y>0)
		glDepthMask(GL_FALSE);
	drawObj(floor_buffer, floor_NumVertices);  // draw the floor

//---------------------------------------Set up Shadow
	if (eye.y > 0) {
		
		if (blendFlag == 1) {
			glEnable(GL_BLEND);														//
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//
		}
		mv = LookAt(eye, at, up);
		mv = mv * N*translation* rotate *accumulate_Rotate;
		glUniform1i(glGetUniformLocation(program, "light"), 0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

		if (sphereFlag == 1) // Filled sphere
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else              // Wireframe sphere
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glDepthMask(GL_TRUE);
		shadowTexture.UseTexture(program, 0);
		if (shadowFlag == 1)
			drawObj(shadow_buffer, sphere_NumVertices);  // draw the shadow
		if (blendFlag == 1) {
			glDisable(GL_BLEND);
		}
//     ------------------------------------set Ground again
	
		mv2 = LookAt(eye, at, up);
		glUniform1i(glGetUniformLocation(program, "light"), lightFlag);
		glUniform1i(glGetUniformLocation(program, "shading"), 0);
		SetUp_Material(floor_ambient, floor_diffuse, floor_specular, material_shininess);
		SetUp_Lighting_Uniform_Vars(mv2);
		mv2 = mv2 * Translate(0.0, 0.0, 0.0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2); // GL_TRUE: matrix is row-major
														 //glUniformMatrix4fv(shadow, 1, GL_TRUE, s);
		if (floorFlag == 1) // Filled floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else              // Wireframe floor
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		normal_matrix = NormalMatrix(mv2, 1);
		glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		if (textureGroundFlag == 1)                   //ground texture 
			floorTexture.UseTexture(program, 2);
		else
			floorTexture.UseTexture(program, 0);
		glDepthMask(GL_TRUE);
		drawObj(floor_buffer, floor_NumVertices);  // draw the floor
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	//glDepthMask(GL_TRUE);
 /*----- Set up the Mode-View matrix for the axis -----*/
	
	mv = LookAt(eye, at, up);
	glUniform1i(glGetUniformLocation(program, "light"), 0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
	//glUniformMatrix4fv(shadow, 1, GL_TRUE, s);
	//glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	axisTexture.UseTexture(program, 0);
	drawObj2(axis_buffer, axis_NumVertices);              //
	
//-------------------------------set up Firework 
	if (particlesFlag == 1) {
		glUseProgram(program_particles);
		mv = LookAt(eye, at, up);

		model_view = glGetUniformLocation(program_particles, "model_view");
		projection = glGetUniformLocation(program_particles, "projection");
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

		
		glUniform1f(glGetUniformLocation(program_particles, "time"), cur_time);

		drawPoints(particles_buffer, particles_N);
	}
	glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle (void)
{
    angle += 1.5;    //YJC: change this value to adjust the cube rotation speed.
	time=(float)glutGet(GLUT_ELAPSED_TIME);
	cur_time = time - sub_time;
	/*if (cur_time >= max_time) {
		cur_time == 0;
		sub_time = time;
	}*/
	cur_time = (int)cur_time % (int)max_time;
    glutPostRedisplay();
}

void setMenu(int id) {
	switch (id) {
	case 1:
		eye = init_eye;
		break;
	case 2:
		exit(0);
		break;
	defalut:
		break;
	}
	glutIdleFunc(idle);
	animationFlag = 1;
}

void shadowMenu(int id) {
	switch (id) {
	case 3:
		shadowFlag = 1; break;
	case 4:
		shadowFlag = 0; break;
	}
}

void wireFrameMenu(int id) {
	switch (id) {
	case 5:
		sphereFlag = 0; textureSphereFlag = 0; break;
	case 6:
		sphereFlag = 1; break;
	}

}

void lightMenu(int id) {
	switch (id) {
	case 7:
		lightFlag = 1; break;
	case 8:
		lightFlag = 0; break;
	}
}

void lightSourceMenu(int id) {
	switch (id) {
	case 9:
		pointLightCount = 1;
		spotLightCount = 0; break;
	case 10:
		pointLightCount = 0;
		spotLightCount = 1; break;
	case 11:
		pointLightCount = 0;
		spotLightCount = 0; break;
	}
}

void shadingMenu(int id) {
	switch (id) {
	case 12:
		shadingFlag = 0; sphereFlag = 1;  break;
	case 13:
		shadingFlag = 1; sphereFlag = 1; break;
	}
}

void fogMenu(int id) {
	switch (id) {
	case 14:fogFlag = 0; break;
	case 15:fogFlag = 1; break;
	case 16:fogFlag = 2; break;
	case 17:fogFlag = 3; break;
	}
}

void blendMenu(int id) {
	switch (id) {
	case 18:blendFlag = 1; break;
	case 19:blendFlag = 0; break;

	}
}

void textureGroundMenu(int id) {
	switch (id) {
	case 20:textureGroundFlag = 1; break;
	case 21:textureGroundFlag = 0; break;
	}
}

void textureSphereMenu(int id) {
	switch (id) {
	case 22:textureSphereFlag = 0;   break;
	case 23:textureSphereFlag = 1; break;
	case 24:textureSphereFlag = 2; break;

	}

}
void FireworkMenu(int id) {
	switch (id) {
	case 25:
		/*if (particlesFlag == 1) {
			sub_time = sub_time + (float)glutGet(GLUT_ELAPSED_TIME) - last_time;
		}
		else {*/
			particlesFlag = 1;
			cur_time = 0;
			sub_time = (float)glutGet(GLUT_ELAPSED_TIME);
		//}
		break;
	case 26:particlesFlag = 0; break;
	}
}
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
		case 033: // Escape Key
		/*case 'q': case 'Q':
			 exit( EXIT_SUCCESS );
				break;*/
			break;
        case 'X': eye[0] += 1.0; break;
		case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
		case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
		case 'z': eye[2] -= 1.0; break;

        case 'B': case 'b': // Toggle between rolling and not rolling
			animationFlag = 1 -  animationFlag;
			if (startFlag == 0) startFlag = 1;
			if (animationFlag == 1) {
				sub_time =sub_time+ ((float)glutGet(GLUT_ELAPSED_TIME) - last_time);
				glutIdleFunc(idle);
			}
			else {
				last_time = (float)glutGet(GLUT_ELAPSED_TIME);

				glutIdleFunc(NULL);
			}
			break;
	   
		case 'S':case's':
			sphereTexture.ChangeTextureWay(1);
			break;
		case 'V':case'v':
			sphereTexture.ChangeTextureWay(2);
			break;
		case 'O':case'o':
			sphereTexture.ChangeTextureSpace(1);
			break;
		case 'E':case'e':
			sphereTexture.ChangeTextureSpace(2);
			break;

		case 'L':case'l':
			sphereTexture.ChangeTextureLattice();
			shadowTexture.ChangeTextureLattice();
			break;
		case'U':case'u':
			sphereTexture.ChangeTextureLatticePattern(1);
			shadowTexture.ChangeTextureLatticePattern(1);
			break;
		case'T':case't':
			sphereTexture.ChangeTextureLatticePattern(2);
			shadowTexture.ChangeTextureLatticePattern(2);
			break;
		 //   case 'c': case 'C': // Toggle between filled and wireframe cube
			//sphereFlag = 1 -  sphereFlag;   
		 //       break;

		 //   case 'f': case 'F': // Toggle between filled and wireframe floor
			//floorFlag = 1 -  floorFlag; 
		 //       break;

		//case ' ':  // reset to initial viewer/eye position
		//    eye = init_eye;
		//    break;
    }
    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		last_time = (float)glutGet(GLUT_ELAPSED_TIME);
		
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (startFlag == 1) {
			animationFlag = 1 - animationFlag;
			if (animationFlag == 1) {
				sub_time = sub_time+(float)glutGet(GLUT_ELAPSED_TIME) - last_time;
				glutIdleFunc(idle);
			}
			else {
				last_time = (float)glutGet(GLUT_ELAPSED_TIME);

				glutIdleFunc(NULL);
			}
		}
	}
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    glutCreateWindow("Sphere");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
#endif
	//pointLight.getInfo();
    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(NULL);
    glutKeyboardFunc(keyboard);
	glutMouseFunc(myMouse);
	

	int subMenu1 = glutCreateMenu(shadowMenu);
	glutAddMenuEntry("Yes", 3);
	glutAddMenuEntry("No", 4);

	int subMenu2 = glutCreateMenu(wireFrameMenu);
	glutAddMenuEntry("Yes", 5);
	glutAddMenuEntry("No", 6);

	int subMenu3 = glutCreateMenu(lightMenu);
	glutAddMenuEntry("Yes", 7);
	glutAddMenuEntry("No", 8);

	int subMenu4 = glutCreateMenu(lightSourceMenu);
	glutAddMenuEntry("Point Source", 9);
	glutAddMenuEntry("Spot Source", 10);
	glutAddMenuEntry("Off", 11);

	int subMenu5 = glutCreateMenu(shadingMenu);
	glutAddMenuEntry("Flat shading", 12);
	glutAddMenuEntry("Smooth shading", 13);

	int subMenu6 = glutCreateMenu(fogMenu);
	glutAddMenuEntry("No fog", 14);
	glutAddMenuEntry("Linear fog", 15);
	glutAddMenuEntry("Exponential fog", 16);
	glutAddMenuEntry("Exponential square", 17);

	int subMenu7 = glutCreateMenu(blendMenu);
	glutAddMenuEntry("Yes", 18);
	glutAddMenuEntry("No", 19);

	int subMenu8 = glutCreateMenu(textureGroundMenu);
	glutAddMenuEntry("Yes", 20);
	glutAddMenuEntry("No", 21);

	int subMenu9 = glutCreateMenu(textureSphereMenu);
	glutAddMenuEntry("NO", 22);
	glutAddMenuEntry("Yes - Contour Lines", 23);
	glutAddMenuEntry("Yes - Checkerboard", 24);

	int subMenu10 = glutCreateMenu(FireworkMenu);
	glutAddMenuEntry("Enable", 25);
	glutAddMenuEntry("Disable", 26);

	glutCreateMenu(setMenu);
	glutAddMenuEntry("default view point", 1);
	glutAddSubMenu("Shadow", subMenu1);
	glutAddSubMenu("Wire Frame Sphere", subMenu2);
	glutAddSubMenu("Enable Light", subMenu3);
	glutAddSubMenu("Light Source", subMenu4);
	glutAddSubMenu("Shading", subMenu5);
	glutAddSubMenu("Fog", subMenu6);
	glutAddSubMenu("Blending shadow", subMenu7);
	glutAddSubMenu("Texture Mapped Ground", subMenu8);
	glutAddSubMenu("Texture Mapped Sphere", subMenu9);
	glutAddSubMenu("Firework", subMenu10);
	glutAddMenuEntry("quit", 2);
	glutAttachMenu(GLUT_LEFT_BUTTON);

    init();
    glutMainLoop();
	int a;
	std::cin >> a;

    return 0;
}
