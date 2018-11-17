#pragma once
#include "glad/glad.h"
#include <SDL_opengl.h>
#include <SDL.h>
#include<iostream>
using namespace std;
struct Shader {
	unsigned int id;        // Shader program id
	bool DEBUG_ON;
	// Vertex attributes locations (default locations)
	int vertexLoc;          // Vertex attribute location point    (default-location = 0)
	int texcoordLoc;        // Texcoord attribute location point  (default-location = 1)
	int normalLoc;          // Normal attribute location point    (default-location = 2)

	// Uniform locations
	int mvpLoc;             // ModelView-Projection matrix uniform location point (vertex shader)
	int colorLoc;           // Diffuse color uniform location point (fragment shader)
	int mapTextureLoc;      // Map texture uniform location point (default-texture-unit = 0)
	Shader(bool debug_on = true) :DEBUG_ON(debug_on) {};
	GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
private:
	static char* readShaderSource(const char* shaderFile);
};
