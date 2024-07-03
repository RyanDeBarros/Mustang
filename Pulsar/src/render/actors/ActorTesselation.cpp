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
	// TODO throughout ActorTesselation, optimize so that the dynamic_cast check only happens at construction, not every frame
	ActorPrimitive2D* primitive = dynamic_cast<ActorPrimitive2D*>(m_Actor);
	if (!primitive)
	{
		if (ActorSequencer2D* sequencer = dynamic_cast<ActorSequencer2D*>(m_Actor))
			primitive = (*sequencer)[i% RenderSeqCount()];
	}
	if (primitive)
		primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i / RenderSeqCount()] ^ m_ActorOffsets[i % RenderSeqCount()]);
	return primitive;
}

BufferCounter ActorTesselation2D::RenderSeqCount() const
{
	if (ActorPrimitive2D* primitive = dynamic_cast<ActorPrimitive2D*>(m_Actor))
		return 1;
	else if (ActorSequencer2D* sequencer = dynamic_cast<ActorSequencer2D*>(m_Actor))
		return sequencer->PrimitiveCount();
	else
		return 0;
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
	if (ActorPrimitive2D* primitive = dynamic_cast<ActorPrimitive2D*>(m_Actor))
		m_ActorOffsets[0] = primitive->GetTransform();
	else if (ActorSequencer2D* sequencer = dynamic_cast<ActorSequencer2D*>(m_Actor))
	{
		sequencer->OnPreDraw();
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			m_ActorOffsets[i] = (*sequencer)[i]->GetTransform();
	}
}

void ActorTesselation2D::OnPostDraw()
{
	if (ActorPrimitive2D* primitive = dynamic_cast<ActorPrimitive2D*>(m_Actor))
		primitive->SetTransform(m_ActorOffsets[0]);
	else if (ActorSequencer2D* sequencer = dynamic_cast<ActorSequencer2D*>(m_Actor))
	{
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			(*sequencer)[i]->SetTransform(m_ActorOffsets[i]);
		sequencer->OnPostDraw();
	}
}
