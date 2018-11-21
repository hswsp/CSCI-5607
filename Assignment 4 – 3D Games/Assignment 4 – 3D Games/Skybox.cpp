#include"Skybox.h"
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGBA,
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

void SkyBox::Render()
{
	m_pSkyboxTechnique->Enable();

	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	/*Pipeline p;
	p.Scale(20.0f, 20.0f, 20.0f);
	p.Rotate(0.0f, 0.0f, 0.0f);
	p.WorldPos(m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);
	p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
	p.SetPerspectiveProj(m_persProjInfo);
	m_pSkyboxTechnique->SetWVP(p.GetWVPTrans());
	m_pCubemapTex->Bind(GL_TEXTURE0);
	m_pMesh->Render();
*/
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
	this->ShaderObj = this->SkyboxShader.InitShader("skybox-Vertex.glsl", "skybox - Fragment.glsl");

	m_WVPLocation = glGetUniformLocation(this->ShaderObj,"gWVP");
	m_textureLocation = glGetUniformLocation(this->ShaderObj,"gCubemapTexture");

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