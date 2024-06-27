#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D* const actor)
	: m_Actor(actor)
{
	m_ActorOffsets.reserve(RenderSeqCount());
	for (BufferCounter i = 0; i < RenderSeqCount(); i++)
		m_ActorOffsets.push_back({});
}

ActorTesselation2D::~ActorTesselation2D()
{
}

void ActorTesselation2D::Insert(const Transform2D& rect)
{
	m_RectVector.push_back(rect);
}

ActorPrimitive2D* const ActorTesselation2D::operator[](const int& i)
{
	if (i >= static_cast<int>(PrimitiveCount()))
		return nullptr;
	ActorPrimitive2D* const primitive = m_Actor->operator[](i % RenderSeqCount());
	primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i / RenderSeqCount()] ^ m_ActorOffsets[i % RenderSeqCount()]);
	return primitive;
}

void ActorTesselation2D::SetZIndex(const ZIndex& z)
{
	m_Actor->SetZIndex(z);
}

ZIndex ActorTesselation2D::GetZIndex() const
{
	return m_Actor->GetZIndex();
}

BufferCounter ActorTesselation2D::PrimitiveCount() const
{
	return static_cast<BufferCounter>(m_RectVector.size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	m_Actor->OnPreDraw();
	for (int i = 0; i < m_ActorOffsets.size(); i++)
		m_ActorOffsets[i] = m_Actor->operator[](i)->GetTransform();
}

void ActorTesselation2D::OnPostDraw()
{
	for (int i = 0; i < m_ActorOffsets.size(); i++)
		m_Actor->operator[](i)->SetTransform(m_ActorOffsets[i]);
	m_Actor->OnPostDraw();
}
