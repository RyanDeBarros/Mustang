#include "render/CanvasLayer.h"

#include "Utility.h"

CanvasLayer::CanvasLayer(ZIndex z)
	: m_Data(CanvasLayerData(z)), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<std::variant<ActorPrimitive2D*, ActorComposite2D*>>>();
}

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<std::variant<ActorPrimitive2D*, ActorComposite2D*>>>();
}

CanvasLayer::~CanvasLayer()
{
	if (m_Batcher)
	{
		m_Batcher->clear();
		delete m_Batcher;
	}
}

void CanvasLayer::OnAttach(const ActorPrimitive2D* const primitive)
{
	// TODO
}

void CanvasLayer::OnAttach(const ActorComposite2D* const composite)
{
	// TODO
}

void CanvasLayer::OnSetZIndex(const ActorPrimitive2D* const primitive, const ZIndex new_val)
{
	// TODO
}

void CanvasLayer::OnSetZIndex(const ActorComposite2D* const composite, const ZIndex new_val)
{
	// TODO
}

void CanvasLayer::OnDetach(const ActorPrimitive2D* const primitive)
{
	// TODO
}

void CanvasLayer::OnDetach(const ActorComposite2D* const composite)
{
	// TODO
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
	// TODO Iterate over m_BatchList, batching subsequences that have the same BatchModel and executing glDrawElements for every batch section. Proper ordering can be assumed. 
}
