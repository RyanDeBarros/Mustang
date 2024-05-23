#include "VertexArray.h"

#include "Utility.h"

static void inline setup_vertices(GLfloat vertices[], VertexCounter& num_vertices, unsigned short& layout_mask, unsigned int& layout)
{
	unsigned short stride = 0;
	unsigned char num_attribs = 0;
	while (layout_mask >> num_attribs != 0)
	{
		auto shift = 2 * num_attribs;
		stride += ((layout & (3 << shift)) >> shift) + 1;
		num_attribs++;
	}
	unsigned short offset = 0;
	num_attribs = 0;
	while (layout_mask >> num_attribs != 0)
	{
		TRY(glEnableVertexAttribArray(num_attribs));
		auto shift = 2 * num_attribs;
		unsigned char attrib = ((layout & (3 << shift)) >> shift) + 1;
		TRY(glVertexAttribPointer(num_attribs, attrib, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (const GLvoid*)offset));
		offset += attrib * sizeof(GLfloat);
		num_attribs++;
	}
	TRY(glBufferData(GL_ARRAY_BUFFER, num_vertices * stride * sizeof(GLfloat), vertices, GL_STATIC_DRAW));
}

static void inline setup_indices(GLuint indices[], VertexCounter num_indices)
{
	TRY(glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices, GL_STATIC_DRAW));
}

VertexArray::VertexArray(GLfloat vertices[], VertexCounter num_vertices, VertexLayoutMask layout_mask, VertexLayout layout, GLuint indices[], VertexCounter num_indices)
	: m_VA(0), m_VB(0), m_IB(0)
{
	TRY(glGenVertexArrays(1, &m_VA));
	TRY(glGenBuffers(1, &m_VB));
	TRY(glGenBuffers(1, &m_IB));
	Bind();
	setup_vertices(vertices, num_vertices, layout_mask, layout);
	setup_indices(indices, num_indices);
	Unbind();
}

VertexArray::~VertexArray()
{
	Unbind();
	TRY(glDeleteBuffers(1, &m_IB));
	TRY(glDeleteBuffers(1, &m_VB));
	TRY(glDeleteVertexArrays(1, &m_VA));
}

void VertexArray::Bind()
{
	TRY(glBindVertexArray(m_VA));
	TRY(glBindBuffer(GL_ARRAY_BUFFER, m_VB));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
}

void VertexArray::Unbind()
{
	TRY(glBindVertexArray(0));
	TRY(glBindBuffer(GL_ARRAY_BUFFER, 0));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
