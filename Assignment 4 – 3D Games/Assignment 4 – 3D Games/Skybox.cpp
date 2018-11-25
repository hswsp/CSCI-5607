#include"Skybox.h"
CubemapTexture::CubemapTexture(const string& PosXFilename,
	const string& NegXFilename,
	const string& PosYFilename,
	const string& NegYFilename,
	const string& PosZFilename,
	const string& NegZFilename)
{
	m_fileNames[0] = PosXFilename;
	m_fileNames[1] = NegXFilename;
	m_fileNames[2] = PosYFilename;
	m_fileNames[3] = NegYFilename;
	m_fileNames[4] = PosZFilename;
	m_fileNames[5] = NegZFilename;
}


bool CubemapTexture::Load()
{
	glGenTextures(1, &m_textureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
	SDL_Surface* surface;
	for (unsigned int i = 0; i < 6; i++) 
	{ 
		surface = SDL_LoadBMP(m_fileNames[i].c_str());

		if (surface == NULL) { //If it failed, print the error
			printf("Error: \"%s\"\n", SDL_GetError());
			return false;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_BGR,
			GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	}
	SDL_FreeSurface(surface);
	return true;
}

void CubemapTexture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}

bool SkyBox::Init(
	const string& PosXFilename,
	const string& NegXFilename,
	const string& PosYFilename,
	const string& NegYFilename,
	const string& PosZFilename,
	const string& NegZFilename)
{
	m_pSkyboxTechnique = new SkyboxTechnique();
	m_pCubemapTex = new CubemapTexture(PosXFilename, NegXFilename, PosYFilename, NegYFilename, PosZFilename, NegZFilename);
	return m_pCubemapTex->Load();
}


void SkyBox::UpdateData(const char* modelpath,GLuint& vao, GLuint vbo[1])
{
	if (!this->m_pSkyboxTechnique->Init())
	{
		fprintf(stderr, "Error creating SkyboxTechnique\n");
	};
	ifstream modelFile;
	modelFile.open(modelpath);
	int numLines = 0;
	modelFile >> numLines;
	vertices = new float[numLines];
	for (int i = 0; i < numLines; i++) {
		modelFile >>vertices[i];
	}
	this->totalNumVerts = numLines / 8;
	modelFile.close();
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, this->totalNumVerts * 8 * sizeof(float), vertices, GL_STATIC_DRAW); //upload vertices to vbo
	
	GLint posAttrib = glGetAttribLocation(m_pSkyboxTechnique->ShaderObj, "Position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//Attribute, vals/attrib., type, isNormalized, stride, offset
	glEnableVertexAttribArray(posAttrib);
	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one	
	glEnable(GL_DEPTH_TEST);
	this->vboId[0] = vbo[0];     // Vertex position VBO
	this->vaoId = vao;
}

void SkyBox::Render(const Camera& camera, glm::mat4 proj)
{
	m_pSkyboxTechnique->Enable();

	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glm::vec3 position = camera.position;
	glm::mat4 model;
	model = glm::mat4();
	glm::vec3 scale(100.0f, 100.0f, 100.0f);
	model = glm::scale(model, scale); 
	glm::mat4 matModelview = camera.getWorldToViewMatrix();
	model = proj * matModelview * model ;
	this->m_pSkyboxTechnique->SetWVP(model);
	m_pCubemapTex->Bind(GL_TEXTURE8);
	//Draw an instance of the model (at the position & orientation specified by the model matrix above)
	int model_start = 0;
	glDrawArrays(GL_TRIANGLES, model_start, totalNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}

SkyboxTechnique::SkyboxTechnique()
{
}

bool SkyboxTechnique::Init()
{
	m_shaderProg = glCreateProgram();
	if (m_shaderProg == 0) {
		fprintf(stderr, "Error creating shader program\n");
		return false;
	}
	this->ShaderObj = this->SkyboxShader.InitShader("skybox-Vertex.glsl", "skybox-Fragment.glsl");

	m_WVPLocation = glGetUniformLocation(this->ShaderObj,"gWVP");
	m_textureLocation = glGetUniformLocation(this->ShaderObj,"gCubemapTexture");
	//m_textureLocation = glGetUniformLocation(this->ShaderObj, "gCubemapTexture");


	if (m_WVPLocation == GL_INVALID_OPERATION ||
		m_textureLocation == GL_INVALID_OPERATION) {
		return false;
	}
	return true;
}


void SkyboxTechnique::SetWVP(const glm::mat4& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, glm::value_ptr(WVP));
}


void SkyboxTechnique::SetTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_textureLocation, TextureUnit);
}


void SkyboxTechnique::Enable()
{
	glUseProgram(m_shaderProg);
}