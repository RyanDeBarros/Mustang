#pragma once

#include "GL/glew.h"

class Shader
{
	GLuint m_RID;
public:
	Shader(const char* vertex_filepath, const char* fragment_filepath);
	~Shader();

	void Bind();
	void Unbind();
};
