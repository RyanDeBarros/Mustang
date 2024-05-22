#include "render/CanvasLayer.h"

#include "Utility.h"

CanvasLayer::CanvasLayer(ZIndex z)
	: m_Data(CanvasLayerData(z))
{
}

CanvasLayer::~CanvasLayer()
{
}

void CanvasLayer::OnDraw()
{
	if (m_Data.enableGLBlend)
	{
		TRY(glEnable(GL_BLEND));
		TRY(glBlendFunc(m_Data.sourceBlend, m_Data.destBlend));
	}
	else
	{
		TRY(glDisable(GL_BLEND));
	}
	// Draw all batches
}
