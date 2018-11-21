#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <SDL_opengl.h>
#include <SDL.h>
#include <fstream>
#include<iostream>
struct Camera
{
	glm::vec3 position;       // Camera position
	glm::vec3 target;         // Camera target it looks-at
	glm::vec3 viewDirection;
	glm::vec3 up;             // Camera up vector (rotation over its axis)
	float fovy;             // Camera field-of-view apperture in Y (degrees)
	glm::vec2 oldMousePosition;
	char key;
	Camera() {};
	Camera(glm::vec3 Aposition, glm::vec3 Atarget, glm::vec3 Aup, float Afovy) :
		position(Aposition), target(Atarget), up(glm::normalize(Aup)), fovy(Afovy), viewDirection(glm::normalize(Atarget- Aposition))
	{}
	glm::mat4 getWorldToViewMatrix() const;
	void mouseUpdate(const glm::vec2& newMousePosition);
};