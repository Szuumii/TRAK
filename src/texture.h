#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "inc.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>
using namespace std;
class Texture2D
{
public:
	unsigned char *pixels;
	unsigned int width, height, depth;
	GLuint OGLTexture;
	bool loaded;
private:
	Texture2D(){}
	Texture2D(const Texture2D &tex){}
public:
	Texture2D(unsigned int width, unsigned int height, unsigned char *pixels)
	{
		this->width = width;
		this->height = height;
		this->pixels = pixels;
		this->depth = 4;
	}
	Texture2D(const char *filename)
	{
		loaded = LoadFromFile(filename);
	}
	~Texture2D()
	{
		if (pixels!=NULL)
			delete []pixels;
	}
	bool LoadFromFile(const char *filename)
	{
        return true;
	}

};
#endif