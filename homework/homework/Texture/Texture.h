#pragma once
#include "../Angel-yjc.h"

#define ImageWidth  64
#define ImageHeight 64
#define	stripeImageWidth 32
class Texture
{
public:
	Texture();

	Texture(GLuint textureObj);

	
	void InitTexture();
	void UseTexture(GLuint program,GLuint textureFlag);
	void ChangeTextureWay(int w);
	void ChangeTextureSpace(int s);
	void ChangeTextureLattice() { textureLatticeFlag = !textureLatticeFlag; }
	void ChangeTextureLatticePattern(int p);
	~Texture();


private:
	GLuint texName;
	GLubyte Image[ImageHeight][ImageWidth][4];
	GLubyte stripeImage[4 * stripeImageWidth];
	int textureWay = 1;    //1: verticle  2: slant
	int textureSpace = 1;   //1: object  2: eye
	int textureObject = 0; //0: nothing 1: sphere  2:ground  3:shadow
	bool textureLatticeFlag = false;
	int textureLatticePattern = 1;  //1: upright 2:  tilted
	void image_set_up(void);
};

