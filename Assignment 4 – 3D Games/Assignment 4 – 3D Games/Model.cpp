#include"Model.h"
#include<sstream>
using namespace std;
const int SKYBOX_WIDTH = 6;
const int SKYBOX_HEIGHT = 6;
const int SKYBOX_LENGTH = 6;
//-------------------------------Light----------------------------
void SetLightUniform(GLuint Shader, const char* propertyName, size_t lightIndex, glm::vec4 position)
{
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	//cout << uniformName.c_str();
	GLint uniPosition = glGetUniformLocation(Shader, uniformName.c_str());
	glUniform4fv(uniPosition, 4, glm::value_ptr(position));
}
void SetLightUniform(GLuint Shader, const char* propertyName, size_t lightIndex, glm::vec3 value)
{
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	//cout << uniformName.c_str();
	GLint uniPosition = glGetUniformLocation(Shader, uniformName.c_str());
	glUniform3fv(uniPosition, 3, glm::value_ptr(value));
}
void SetLightUniform(GLuint Shader, const char* propertyName, size_t lightIndex,float value)
{
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	//cout << uniformName.c_str();
	GLint uniPosition = glGetUniformLocation(Shader, uniformName.c_str());
	glUniform1i(uniPosition, value);
}
void SetLightUniform(GLuint Shader, const char* structName,const Light& light)//, glm::mat4 matModelview
{
	std::ostringstream ss;
	ss << "position"<< structName;//<< structName << "." 
	std::string uniformName = ss.str();
	GLint uniPosition = glGetUniformLocation(Shader, uniformName.c_str());
	/*if (light.position.w == 0)
	{
		glm::vec4 Direction(matModelview * light.position);
		glUniform4fv(uniPosition, 4, glm::value_ptr(Direction));
	}*/
	//else
	glUniform4fv(uniPosition, 1, glm::value_ptr(light.position));
	ss.str("");

	ss <<"intensities" << structName;
	std::string uniintensities = ss.str();
	uniPosition = glGetUniformLocation(Shader, uniintensities.c_str());
	glUniform3fv(uniPosition, 1, glm::value_ptr(light.intensities));
	ss.str("");

	ss  << "coneDirection" << structName;
	std::string uniconeDirection = ss.str();
	uniPosition = glGetUniformLocation(Shader, uniconeDirection.c_str());
	//glm::vec3  coneDirection(matModelview * glm::vec4(light.coneDirection, 0.0));
	glUniform3fv(uniPosition, 1, glm::value_ptr(light.coneDirection));//
	ss.str("");

	ss << "attenuation" << structName;
	std::string uniattenuation = ss.str();
	uniPosition = glGetUniformLocation(Shader, uniattenuation.c_str());
	glUniform1f(uniPosition, light.attenuation);
	ss.str("");

	ss  << "ambientCoefficient" << structName;
	std::string uniambientC = ss.str();
	uniPosition = glGetUniformLocation(Shader, uniambientC.c_str());
	glUniform1f(uniPosition, light.ambientCoefficient);
	ss.str("");

	ss << "coneAngle" << structName;
	std::string uniconeAngle = ss.str();
	uniPosition = glGetUniformLocation(Shader, uniconeAngle.c_str());
	glUniform1f(uniPosition, light.coneAngle);
}


void Object::UploadPosition(glm::vec3 position)
{
	this->objx = float(position.x);
	this->objy = float(position.y);
	this->objz = float(position.z);
}


void Model::InitModel(const int AtotalNumModels)
{
	//obj = new Object*[modelNum];
	totalNumVerts = 0;
	this->MaxmodelNum = AtotalNumModels;
	obj.reserve(AtotalNumModels);
}
void Model::ImportModel(const char* modelpath, Object* objects)//int k
{
	//assert(k < this->modelNum);
	ifstream modelFile;
	modelFile.open(modelpath);
	int numLines = 0;
	modelFile >> numLines;
	objects->vertices = new float[numLines];//this->obj[k]
	for (int i = 0; i < numLines; i++) {
		modelFile >> objects->vertices[i];//this->obj[k]
	}
	//printf("%d\n", numLines);
	objects->vertexCount = numLines / 8;//this->obj[k]
	this->totalNumVerts+= numLines / 8;
	modelFile.close();
	this->obj.push_back(objects);
	//this->MaxmodelNum++;
}

void Model::UploadMeshData(GLuint& vao, GLuint vbo[3])
{
	/*int totalNumVerts = 0;
	for (int i = 0; i < this->modelNum; ++i)
		totalNumVerts += this->vertexCount[i];*/
	this->LoadLight();
	float* modelData = new float[totalNumVerts * 8];
	int startModeli = 0;
	for (vector<Object *>::iterator it = obj.begin(); it != obj.end(); ++it)//int i = 0; i < this->obj.size
	{
		//this->obj[i]
		copy((*it)->vertices, (*it)->vertices + (*it)->vertexCount * 8, modelData+ startModeli*8);
		startModeli += (*it)->vertexCount;
	}
	
	// Build a Vertex Array Object(VAO) to store mapping of shader attributse to VBO
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, totalNumVerts * 8 * sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
	//Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//Attribute, vals/attrib., type, isNormalized, stride, offset
	glEnableVertexAttribArray(posAttrib);

	//GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);

	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	this->uniView = glGetUniformLocation(texturedShader, "view");
	this->uniProj = glGetUniformLocation(texturedShader, "proj");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one	


	glEnable(GL_DEPTH_TEST);

	this->vboId[0] = vbo[0];     // Vertex position VBO
	this->vboId[1] = vbo[1];     // Texcoords VBO
	this->vboId[2] = vbo[2];     // Normals VBO
	this->vaoId = vao;
}
void Model::LoadModel(glm::mat4 view, glm::mat4 proj)
{
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	for (int i = 0; i < this->obj.size(); ++i)
	{
		//only have 2 now
		if (this->obj[i]->texture->textureUnit == 1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->obj[i]->texture->id);
			glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);
		}
		else if (this->obj[i]->texture->textureUnit == 2)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->obj[i]->texture->id);
			glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);
		}
		if (this->obj[i]->texture->textureUnit == 3)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, this->obj[i]->texture->id);
			glUniform1i(glGetUniformLocation(texturedShader, "tex2"), 2);
		}
		else if (this->obj[i]->texture->textureUnit == 4)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, this->obj[i]->texture->id);
			glUniform1i(glGetUniformLocation(texturedShader, "tex3"), 3);
		}
	}
	glBindVertexArray(vaoId);
}
void Model::DrawModel(int k, Camera camera, glm::vec3 scale) //bool IsScale
{
	if (!this->obj[k]->display)
		return;
	//View
	GLint uniView = glGetUniformLocation(texturedShader, "view");
	glm::mat4 matModelview = camera.getWorldToViewMatrix();//glm::lookAt(camera.position, camera.target, camera.up)
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(matModelview));

	float colR = 1, colG = 1, colB = 1;
	GLint uniColor = glGetUniformLocation(texturedShader, "inColor");
	glm::vec3 colVec(colR, colG, colB);
	glUniform3fv(uniColor, 3, glm::value_ptr(colVec));
	/*parameter:	
	¡¤location ¨C the previously queried location
	¡¤count ¨C the number of elements in the array
	¡¤v ¨C an array of floats*/
		
	GLint uniTexID = glGetUniformLocation(texturedShader, "texID");
	GLint uniModel = glGetUniformLocation(texturedShader, "model");
	//-------------------------Light-------------------------
	GLint uniLights = glGetUniformLocation(texturedShader, "numLights");
	glUniform1i(uniLights, (int)gLights.size());
	//spot light
	glm::vec3 playerPos((0.02*camera.position.x) / float(SKYBOX_WIDTH), (0.3*camera.position.y) / float(SKYBOX_HEIGHT) ,
		camera.position.z + SKYBOX_LENGTH / 2 - 2.0f);//-0.5 5.5f -0.5 2.5f  + SKYBOX_LENGTH /2-2.0f
	//- 0.9*SKYBOX_WIDTH - 0.3*SKYBOX_HEIGHT 
	gLights[0].position = glm::vec4(playerPos, 1.0);
	gLights[0].coneDirection = camera.viewDirection;
	/*for (size_t i = 0; i < gLights.size(); ++i) 
	{
		if (gLights[i].position.w == 0)
		{
			glm::vec4 Direction(matModelview * gLights[i].position);
			SetLightUniform(texturedShader, "position", i, Direction);
		}
		else
			SetLightUniform(texturedShader, "position", i, gLights[i].position);

		SetLightUniform(texturedShader, "intensities", i, gLights[i].intensities);
		SetLightUniform(texturedShader, "attenuation", i, gLights[i].attenuation);
		SetLightUniform(texturedShader, "ambientCoefficient", i, gLights[i].ambientCoefficient);
		SetLightUniform(texturedShader, "coneAngle", i, gLights[i].coneAngle);
		glm::vec3  coneDirection(matModelview * glm::vec4(gLights[i].coneDirection,0.0));
		SetLightUniform(texturedShader, "coneDirection", i, coneDirection);
	}*/
	SetLightUniform(texturedShader, "0", gLights[0]);//, matModelview
	SetLightUniform(texturedShader, "1", gLights[1]);//, matModelview

	/*GLint uniCameraPos = glGetUniformLocation(texturedShader, "cameraPosition");
	glUniform3fv(uniCameraPos, 1, glm::value_ptr(playerPos));*/

	GLint uniShininess = glGetUniformLocation(texturedShader, "materialShininess");
	glUniform1f(uniShininess, 4.0);
	GLint uniSpecularColor = glGetUniformLocation(texturedShader, "materialSpecularColor");
	glm::vec3 shiness(1.0, 1.0, 1.0);
	glUniform3fv(uniSpecularColor, 1, glm::value_ptr(shiness));

	//--------------------------------------------------------------------------
	glm::mat4 model;
	model = glm::mat4();
	// cube need scale
	model = glm::scale(model, scale); //scale this model glm::vec3 (.2f, 1.8f, 1.8f)
	model = glm::translate(model, glm::vec3(this->obj[k]->objx, this->obj[k]->objy, this->obj[k]->objz));

	//Set which texture to use (1 = brick texture ... bound to GL_TEXTURE1)
	glUniform1i(uniTexID,this->obj[k]->texture->id);
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	


	//Draw an instance of the model (at the position & orientation specified by the model matrix above)
	int model_start = 0;
	for (int i = 0; i < k; ++i)
		model_start += this->obj[i]->vertexCount;
	glDrawArrays(GL_TRIANGLES, model_start, this->obj[k]->vertexCount); //(Primitive Type, Start Vertex, Num Verticies)
	
}

void Model::LoadLight()
{
	//-------------------------------light------------------------
	// setup lights
	Light spotlight;
	spotlight.position = glm::vec4(-4, 0, 10, 1);
	spotlight.intensities = glm::vec3(2, 2, 2); //strong white light
	spotlight.attenuation = 0.1f;
	spotlight.ambientCoefficient = 0.0f; //no ambient light
	spotlight.coneAngle = 20.0f;
	spotlight.coneDirection = glm::vec3(0, 0, -1);

	Light directionalLight;
	directionalLight.position = glm::vec4(1, 1, -1, 0); //w == 0 indications a directional light 1, 0.8, 0.6,
	directionalLight.intensities = glm::vec3(0.4, 0.3, 0.1); //weak yellowish light
	directionalLight.ambientCoefficient = 0.3;

	gLights.push_back(spotlight);
	gLights.push_back(directionalLight);

	//GLint uniLights = glGetUniformLocation(texturedShader, "numLights");
	//glUniform1i(uniLights, (int)gLights.size());
	//for (size_t i = 0; i < gLights.size(); ++i) {
	//	SetLightUniform(texturedShader, "position", i, gLights[i].position);
	//	SetLightUniform(texturedShader, "intensities", i, gLights[i].intensities);
	//	SetLightUniform(texturedShader, "attenuation", i, gLights[i].attenuation);
	//	SetLightUniform(texturedShader, "ambientCoefficient", i, gLights[i].ambientCoefficient);
	//	SetLightUniform(texturedShader, "coneAngle", i, gLights[i].coneAngle);
	//	SetLightUniform(texturedShader, "coneDirection", i, gLights[i].coneDirection);
	//}
	//-----------------------------------------------------------------------------------
}