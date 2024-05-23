#include "render/CanvasLayer.h"

#include "Utility.h"

CanvasLayer::CanvasLayer(ZIndex z)
	: m_Data(CanvasLayerData(z)), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_BatchList = new std::forward_list<ActorPrimitive2D>();
}

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_BatchList = new std::forward_list<ActorPrimitive2D>();
}

CanvasLayer::~CanvasLayer()
{
	if (m_BatchList)
	{
		m_BatchList->clear();
		delete m_BatchList;
	}
}

CanvasHandle CanvasLayer::OnAttach(ActorPrimitive2D* primitive)
{
	// TODO Add primitives in correct order, following 1: ZIndex, 2: order set by m_Data.relativeOrderByInsert. Use indexing pointers that point to edges of ZIndex sections for faster insertion.
}

CanvasHandle CanvasLayer::OnAttach(ActorConstruct2D* construct)
{
	// TODO Add primitives in correct order, following 1: ZIndex, 2: order set by m_Data.relativeOrderByInsert. Use indexing pointers that point to edges of ZIndex sections for faster insertion.
}

void CanvasLayer::OnSetZIndex(const CanvasHandle& handle, ZIndex new_val)
{
	// TODO Reorder the primitive OR construct at the handle. Update indexing pointers as necessary.
}

void CanvasLayer::OnDetach(const CanvasHandle& handle)
{
	// TODO Remove primitive OR construct at the handle. Update indexing pointers as necessary.
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
