#pragma once

#include "GL/glew.h"

#include "VertexArray.h"
#include "Shader.h"

class Renderable
{
	GLuint m_RID;
	int m_Zindex;
	
public:
	Renderable(const char* asset_filepath);
	~Renderable();

	void OnDraw();
	
	int GetZIndex() const { return m_Zindex; }
private:
	void LoadAsset(const char* asset_filepath);
};
