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
#include"Texture.h"
#include"Camera.h"
#include"Light.h"
using namespace std;
enum ObjType
{
	NONE,
	FLOOR,
	WALL,
	KEY,
	DOOR,
	EXIT
};
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
	//---------------------------map object to certain map------------
	bool display;
	bool Isbind;
	ObjType type;
	//-----------------------------------texture---------------------------
	Texture2D* texture;
	Object(ObjType Atype=NONE, bool bind=false, bool disp = true):type(Atype),display(disp),Isbind(bind){}
	Object(Texture2D& Atexture, ObjType Atype = NONE, bool bind = false, bool disp = true, float x = 0, float y = 0, float z = 0):
		objx(x), objy(y), objz(z), type(Atype), Isbind(bind), display(disp)
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
	void UploadPosition(glm::vec3 position);
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
	int MaxmodelNum;              //total model number
	//Object ** obj;
	vector<Object *> obj;
	GLuint vaoId;     // OpenGL Vertex Array Object id
	GLuint vboId[3];  // OpenGL Vertex Buffer Objects id (3 types of vertex data supported)
	GLuint texturedShader;
	GLint uniView;
	GLint uniProj;
	//------------------------Light control------------------------------------------------------------------
	vector<Light> gLights;
	/*Model(Texture2D** Atexture, float x=0, float y=0, float z=0): 
		texture(Atexture), objx(x), objy(y), objz(z){};*/
	void InitModel(const int AtotalNumVerts = 0);
	Model(int num= 1)
	{
		InitModel(num);
		/*vertexCount = new int[num];
		vertices = new float*[num];
		texture = new Texture2D*[num];*/
	};
	void LoadLight();
	void ImportModel(const char* modelpath, Object* objects);//int k kth model
	void UploadMeshData(GLuint& vao, GLuint vbo[3]);
	void LoadModel(glm::mat4 view, glm::mat4 proj);
	void DrawModel(int k, Camera camera, glm::vec3 scale= glm::vec3(1.f,1.f,1.f)); //draw kth model    bool IsScale
	//------------------Light-----------------------------
     
	~Model()
	{
		//for (int i = 0; i < modelNum; ++i)
		//	//delete[] vertices[i];
		//	obj[i]->~Object();
		/*delete[] vertices;
		delete[] vertexCount;*/
		for (auto it = obj.begin(); it != obj.end(); it++)
		{
			delete *it;
		}
		//delete[] obj;
	};
private:
	int totalNumVerts;
	
};
