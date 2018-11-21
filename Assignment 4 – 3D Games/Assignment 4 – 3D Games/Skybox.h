#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <SDL_opengl.h>
#include <SDL.h>
#include <fstream>
#include<iostream>
#include<vector>
#include <list>
#include"shader.h"
#include"Camera.h"
using namespace std;
class CubemapTexture
{
public:

	CubemapTexture(const string& PosXFilename,
		const string& NegXFilename,
		const string& PosYFilename,
		const string& NegYFilename,
		const string& PosZFilename,
		const string& NegZFilename);
	~CubemapTexture();
	bool Load();
	void Bind(GLenum TextureUnit);

private:
	string m_fileNames[6];
	GLuint m_textureObj;
};

class SkyboxTechnique
{
public:
	GLuint m_shaderProg;
	void Enable();
	SkyboxTechnique();
    bool Init();
	void SetWVP(const glm::mat4& WVP);
	void SetTextureUnit(unsigned int TextureUnit);
	GLuint ShaderObj;
private:
	Shader SkyboxShader;
	GLuint m_WVPLocation;
	GLuint m_textureLocation;
};

class SkyBox
{
public:
//	SkyBox(const Camera* pCamera, const PersProjInfo& p);

	~SkyBox();

	bool Init(const string& Directory,
		const string& PosXFilename,
		const string& NegXFilename,
		const string& PosYFilename,
		const string& NegYFilename,
		const string& PosZFilename,
		const string& NegZFilename);
	void Render();
private:
	SkyboxTechnique* m_pSkyboxTechnique;
	const Camera* m_pCamera;
	CubemapTexture* m_pCubemapTex;
	//Mesh* m_pMesh;
	//PersProjInfo m_persProjInfo;
};