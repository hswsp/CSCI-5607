#pragma once
#include "glad/glad.h"
#include <SDL_opengl.h>
#include <SDL.h>
#include<iostream>
//	Image struct (extended)
struct Image {
	unsigned int width;         // Image base width
	unsigned int height;        // Image base height
	unsigned int format;        // Data format (TextureFormat type)
	unsigned char *data;        // Image raw data
	Image(unsigned int Awidth, unsigned int Aheight) :width(Awidth),height(Aheight){}
	
} ;


// NOTE: Texture data is stored in GPU memory (VRAM)
struct Texture2D {
	int id;        // OpenGL texture id
	int width;              // Texture base width
	int height;             // Texture base height
	bool mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (TextureFormat)
	GLuint textureUnit; //texUnit
	
	Texture2D() {};
	Texture2D(int textureid,int AtextureUnit, bool Ismipmap = true) 
		:id(textureid),textureUnit(AtextureUnit),mipmaps(Ismipmap) {}
	Texture2D(Texture2D& Atexture):
		id(Atexture.id), width(Atexture.width),height(Atexture.height),mipmaps(Atexture.mipmaps),
	format(Atexture.format), textureUnit(Atexture.textureUnit){}


	void LoadTexture(const char * filename);
	//~Texture2D() { }
};