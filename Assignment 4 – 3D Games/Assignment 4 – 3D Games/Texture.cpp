#include"Texture.h"
using namespace std;
void Texture2D::LoadTexture(const char * filename)
{
	SDL_Surface* surface;
	surface = SDL_LoadBMP(filename);
	if (surface == NULL) { //If it failed, print the error
		printf("Error: \"%s\"\n", SDL_GetError());
		return;
	}
	this->height = surface->h;
	this->width = surface->w;
	glGenTextures(1, &textureUnit);

	glActiveTexture(GL_TEXTURE0 + textureUnit - 1);
	glBindTexture(GL_TEXTURE_2D, textureUnit - 1);

	//What to do outside 0-1 range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load the texture into memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
	if(this->mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
	SDL_FreeSurface(surface);
}