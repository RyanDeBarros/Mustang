#pragma once

#include "render/Renderable.h"

struct QuadRender : public Renderable
{
	//BatchModel m_Model;
	//GLfloat* m_VertexBufferData;
	//VertexCounter m_VertexCount;
	//GLuint* m_IndexBufferData;
	//VertexCounter m_IndexCount;
	QuadRender(BatchModel model)
	{
		m_Model = model;
		// Using m_Model.layout and m_Model.layoutMask, instantiate m_VertexBufferData, using size properties perhaps. Emphasize use of size over transform.scale, which Renderable does not have access to.
		m_VertexCount = 4;
		m_IndexBufferData = new GLuint[6]{
			0, 1, 2,
			2, 3, 0
		};
		m_IndexCount = 6;
	}
};
