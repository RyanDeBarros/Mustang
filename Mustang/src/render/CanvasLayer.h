#pragma once

#include <GL/glew.h>

#include "Typedefs.h"

struct CanvasLayerData
{
	ZIndex z;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	CanvasLayerData(ZIndex z) : z(z), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA) {}
};

class CanvasLayer
{
	CanvasLayerData m_Data;
private:
	CanvasLayer() : m_Data(CanvasLayerData(0)) {}
public:
	CanvasLayer(ZIndex);
	~CanvasLayer();

	void OnDraw();
	
	ZIndex getZIndex() const { return m_Data.z; }
};
