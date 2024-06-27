#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D* const actor)
	: m_Actor(actor)
{
	if (INHERITS(*m_Actor, ActorPrimitive2D))
	{
		m_ActorOffsets.reserve(1);
		m_ActorOffsets.push_back({});
	}
	else if (INHERITS(*m_Actor, ActorSequencer2D))
	{
		m_ActorOffsets.reserve(RenderSeqCount());
		for (BufferCounter i = 0; i < RenderSeqCount(); i++)
			m_ActorOffsets.push_back({});
	}
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
	if (INHERITS(*m_Actor, ActorPrimitive2D))
	{
		ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(m_Actor);
		primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i] ^ m_ActorOffsets[0]);
		return primitive;
	}
	else if (INHERITS(*m_Actor, ActorSequencer2D))
	{
		ActorPrimitive2D* const primitive = static_cast<ActorSequencer2D* const>(m_Actor)->operator[](i % RenderSeqCount());
		primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i / RenderSeqCount()] ^ m_ActorOffsets[i % RenderSeqCount()]);
		return primitive;
	}
}

void ActorTesselation2D::SetZIndex(const ZIndex& z)
{
	if (INHERITS(*m_Actor, ActorPrimitive2D))
		static_cast<ActorPrimitive2D* const>(m_Actor)->m_Z = z;
	else if (INHERITS(*m_Actor, ActorSequencer2D))
		static_cast<ActorSequencer2D* const>(m_Actor)->SetZIndex(z);
}

ZIndex ActorTesselation2D::GetZIndex() const
{
	if (INHERITS(*m_Actor, ActorPrimitive2D))
		return static_cast<ActorPrimitive2D* const>(m_Actor)->m_Z;
	else if (INHERITS(*m_Actor, ActorSequencer2D))
		return static_cast<ActorSequencer2D* const>(m_Actor)->GetZIndex();
}

BufferCounter ActorTesselation2D::PrimitiveCount() const
{
	if (INHERITS(*m_Actor, ActorPrimitive2D))
		return static_cast<BufferCounter>(m_RectVector.size());
	else if (INHERITS(*m_Actor, ActorSequencer2D))
		return static_cast<BufferCounter>(m_RectVector.size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	if (INHERITS(*m_Actor, ActorPrimitive2D))
		m_ActorOffsets[0] = static_cast<ActorPrimitive2D* const>(m_Actor)->GetTransform();
	else if (INHERITS(*m_Actor, ActorSequencer2D))
	{
		static_cast<ActorSequencer2D* const>(m_Actor)->OnPreDraw();
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			m_ActorOffsets[i] = static_cast<ActorSequencer2D* const>(m_Actor)->operator[](i)->GetTransform();
	}
}

void ActorTesselation2D::OnPostDraw()
{	
	if (INHERITS(*m_Actor, ActorPrimitive2D))
		static_cast<ActorPrimitive2D* const>(m_Actor)->SetTransform(m_ActorOffsets[0]);
	else if (INHERITS(*m_Actor, ActorSequencer2D))
	{
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			static_cast<ActorSequencer2D* const>(m_Actor)->operator[](i)->SetTransform(m_ActorOffsets[i]);
		static_cast<ActorSequencer2D* const>(m_Actor)->OnPostDraw();
	}
}
