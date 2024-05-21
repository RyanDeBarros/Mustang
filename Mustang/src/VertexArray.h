#pragma once

#include "GL/glew.h"

class VertexArray
{
	GLuint m_VA, m_VB, m_IB;
public:
	VertexArray(GLfloat vertices[], size_t num_vertices, unsigned short layout_mask, unsigned int layout, GLuint indices[], size_t num_indices);
	~VertexArray();

	void Bind();
	void Unbind();
};
