#include "Texture.h"



Texture::Texture()
{
	texName = 0;
}

Texture::Texture(GLuint textureObj) {
	texName = 0;
	textureObject = textureObj;

}

void Texture::image_set_up(void)
{
	int i, j, c;
	
	/* --- Generate checkerboard image to the image array ---*/
	for (i = 0; i < ImageHeight; i++)
		for (j = 0; j < ImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

			if (c == 1) /* white */
			{
				c = 255;
				Image[i][j][0] = (GLubyte)c;
				Image[i][j][1] = (GLubyte)c;
				Image[i][j][2] = (GLubyte)c;
			}
			else  /* green */
			{
				Image[i][j][0] = (GLubyte)0;
				Image[i][j][1] = (GLubyte)150;
				Image[i][j][2] = (GLubyte)0;
			}

			Image[i][j][3] = (GLubyte)255;
		}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Generate 1D stripe image to array stripeImage[] ---*/
	for (j = 0; j < stripeImageWidth; j++) {
		/* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
		When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
		*/
		stripeImage[4 * j] = (GLubyte)255;
		stripeImage[4 * j + 1] = (GLubyte)((j>4) ? 255 : 0);
		stripeImage[4 * j + 2] = (GLubyte)0;
		stripeImage[4 * j + 3] = (GLubyte)255;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/*----------- End 1D stripe image ----------------*/

	/*--- texture mapping set-up is to be done in
	init() (set up texture objects),
	display() (activate the texture object to be used, etc.)
	and in shaders.
	---*/


}

void Texture::InitTexture()
{
	image_set_up();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Create and Initialize a texture object ---*/
	glGenTextures(1, &texName);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
	glBindTexture(GL_TEXTURE_2D, texName); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	//
	
	glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1
	glBindTexture(GL_TEXTURE_1D, texName); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth,
		0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
}

void Texture::UseTexture(GLuint program, GLuint textureFlag)    //textureFlag==0  : No texture ;   1: 1D  ;  2: 2D
{
	glUniform1i(glGetUniformLocation(program, "_texture.textureFlag"), textureFlag);
	glUniform1i(glGetUniformLocation(program, "_texture.texture_way"), textureWay);
	glUniform1i(glGetUniformLocation(program, "_texture.texture_space"), textureSpace);
	glUniform1i(glGetUniformLocation(program, "_texture.texture_object"), textureObject);
	glUniform1i(glGetUniformLocation(program, "_texture.texture_lattice"), textureLatticeFlag);
	glUniform1i(glGetUniformLocation(program, "_texture.texture_lattice_pattern"), textureLatticePattern);
	if (textureFlag == 0) {
		return;
	}
	else if(textureFlag==1){
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
	}
	else if (textureFlag == 2) {
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
	}
}

void Texture::ChangeTextureWay(int w)
{
	textureWay = w;
}

void Texture::ChangeTextureSpace(int s)
{
	textureSpace = s;
}


void Texture::ChangeTextureLatticePattern(int p)
{
	textureLatticePattern = p;
}

Texture::~Texture()
{
}
