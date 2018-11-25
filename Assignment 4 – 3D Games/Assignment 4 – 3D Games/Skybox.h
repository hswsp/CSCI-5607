#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include"shader.h"
#include"Camera.h"
#include"Model.h"
#include <SDL_opengl.h>
#include <SDL.h>
#include <fstream>
#include<iostream>
#include<vector>
#include <list>
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
	bool Load();
	void Bind(GLenum TextureUnit);
	~CubemapTexture()
	{
		delete[] m_fileNames;
	}
	/*void SetWVP(const Camera& camera, glm::mat4 model);
	void SetPositoin(const Camera& camera, glm::vec3 model);*/
private:
	string m_fileNames[6];
	GLuint m_textureObj;
};

class SkyboxTechnique
{
public:
	GLuint m_shaderProg;
	void Enable();
	void Disable();
	SkyboxTechnique();
    bool Init();
	//void SetWVP(const glm::mat4& WVP);
	void SetModel(const glm::mat4& WVP);
	void SetView(const glm::mat4& WVP);
	void SetProj(const glm::mat4& WVP);
	void SetTextureUnit(unsigned int TextureUnit);
	//GLuint ShaderObj;
private:
	Shader SkyboxShader;
	//GLuint m_WVPLocation;
	GLuint m_ModelPLocation;
	GLuint m_ViewLocation;
	GLuint m_ProjLocation;
	GLuint m_textureLocation;
	GLuint m_PositionLocation;
};

class SkyBox
{
public:
//	SkyBox(const Camera* pCamera, const PersProjInfo& p);
	GLuint vaoId;     // OpenGL Vertex Array Object id
	GLuint vboId[1];  // OpenGL Vertex Buffer Objects id (3 types of vertex data supported)
	bool Init(
		const string& PosXFilename,
		const string& NegXFilename,
		const string& PosYFilename,
		const string& NegYFilename,
		const string& PosZFilename,
		const string& NegZFilename);
	void UpdateData(const char* modelpath, GLuint& vao, GLuint vbo[1]);
	void Render(const Camera& camera, glm::mat4 proj);
	~SkyBox()
	{
	//	delete m_pSkyboxTechnique;
	//	delete m_pCamera;  
	//	delete m_pCubemapTex;
	}
private:
	SkyboxTechnique* m_pSkyboxTechnique;
	//const Camera* m_pCamera;
	CubemapTexture* m_pCubemapTex;
	int totalNumVerts;
	float *vertices;
	//PersProjInfo m_persProjInfo;
};