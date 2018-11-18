#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <SDL_opengl.h>
#include <SDL.h>
#include <fstream>
#include<iostream>
#include"Texture.h"
#include"Camera.h"
struct Object
{
	int vertexCount;        // number of vertices stored in arrays
	float *vertices;        // vertex position (XYZ - 3 components per vertex) (shader-location = 0)
	//not used yet
	float *texcoords;       // vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
	float *normals;         // vertex normals (XYZ - 3 components per vertex) (shader-location = 2)

	float objx;
	float objy;
	float objz;
	Texture2D* texture;
	Object() {}
	Object(Texture2D& Atexture,float x = 0, float y = 0, float z = 0):objx(x), objy(y), objz(z) 
	{
		texture = new Texture2D(Atexture);
	}
	Object& operator=(const Object& Aobj)
	{
		Object obj;
		this->texture=new Texture2D(*Aobj.texture);
		this->objx = Aobj.objx;
		this->objy = Aobj.objy;
		this->objz = Aobj.objz;
		this->vertexCount = Aobj.vertexCount;
		return *this;
	}
	/*Object(Object& obj) :vertexCount(obj.vertexCount), vertices(obj.vertices),
		texcoords(obj.texcoords), normals(obj.normals), objx(obj.objx), objy(obj.objy), objz(obj.objz) 
	{
		texture = new Texture2D(*obj.texture);
	}*/
	~Object()
	{
		delete[] vertices;
	}

};
struct Model 
{
	int modelNum;              //total model number
	//int *vertexCount;        // number of vertices stored in arrays
	//float **vertices;        // vertex position (XYZ - 3 components per vertex) (shader-location = 0)
	////not used yet
	//float *texcoords;       // vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
	//float *normals;         // vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
	//float objx;
	//float objy;
	//float objz;
	//Texture2D** texture;
	Object ** obj;
	GLuint vaoId;     // OpenGL Vertex Array Object id
	GLuint vboId[3];  // OpenGL Vertex Buffer Objects id (3 types of vertex data supported)
	GLuint texturedShader;

	GLint uniView;
	GLint uniProj;
	
	Model(int num=1) :modelNum(num)
	{
		obj = new Object*[num];
		totalNumVerts = 0;
		/*vertexCount = new int[num];
		vertices = new float*[num];
		texture = new Texture2D*[num];*/
	};
	/*Model(Texture2D** Atexture, float x=0, float y=0, float z=0): 
		texture(Atexture), objx(x), objy(y), objz(z){};*/
	void LoadModel(const char* modelpath,int k);//kth model
	void UploadMeshData(GLuint& vao, GLuint vbo[3]);
	void LoadModel(glm::mat4 view, glm::mat4 proj);
	void DrawModel(int k, Camera camera,bool IsScale); //draw kth model
	~Model()
	{
		//for (int i = 0; i < modelNum; ++i)
		//	//delete[] vertices[i];
		//	obj[i]->~Object();
		/*delete[] vertices;
		delete[] vertexCount;*/
		delete[] obj;
	};
private:
	int totalNumVerts;
	
};
