#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D_P actor)
	: m_Actor(actor)
{
	if (m_Actor.index() == 0)
	{
		m_ActorOffsets.reserve(1);
		m_ActorOffsets.push_back({});
	}
	else
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

ActorPrimitive2D* ActorTesselation2D::operator[](const int& i)
{
	if (i >= static_cast<int>(PrimitiveCount()))
		return nullptr;
	if (m_Actor.index() == 0)
	{
		auto& primitive = std::get<ActorPrimitive2D* const>(m_Actor);
		primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i] ^ m_ActorOffsets[0]);
		return primitive;
	}
	else
	{
		auto const& primitive = std::get<ActorSequencer2D* const>(m_Actor)->operator[](i % RenderSeqCount());
		primitive->SetTransform(m_GlobalTransform ^ m_RectVector[i / RenderSeqCount()] ^ m_ActorOffsets[i % RenderSeqCount()]);
		return primitive;
	}
}

void ActorTesselation2D::SetZIndex(const ZIndex& z)
{
	if (m_Actor.index() == 0)
		std::get<ActorPrimitive2D* const>(m_Actor)->m_Z = z;
	else
		std::get<ActorSequencer2D* const>(m_Actor)->SetZIndex(z);
}

ZIndex ActorTesselation2D::GetZIndex() const
{
	if (m_Actor.index() == 0)
		return std::get<ActorPrimitive2D* const>(m_Actor)->m_Z;
	else
		return std::get<ActorSequencer2D* const>(m_Actor)->GetZIndex();
}

BufferCounter ActorTesselation2D::PrimitiveCount() const
{
	if (m_Actor.index() == 0)
		return static_cast<BufferCounter>(m_RectVector.size());
	else
		return static_cast<BufferCounter>(m_RectVector.size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	if (m_Actor.index() == 0)
		m_ActorOffsets[0] = std::get<ActorPrimitive2D* const>(m_Actor)->GetTransform();
	else
	{
		std::get<ActorSequencer2D* const>(m_Actor)->OnPreDraw();
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			m_ActorOffsets[i] = std::get<ActorSequencer2D* const>(m_Actor)->operator[](i)->GetTransform();
	}
}

void ActorTesselation2D::OnPostDraw()
{
	if (m_Actor.index() == 0)
		std::get<ActorPrimitive2D* const>(m_Actor)->SetTransform(m_ActorOffsets[0]);
	else
	{
		for (int i = 0; i < m_ActorOffsets.size(); i++)
			std::get<ActorSequencer2D* const>(m_Actor)->operator[](i)->SetTransform(m_ActorOffsets[i]);
		std::get<ActorSequencer2D* const>(m_Actor)->OnPostDraw();
	}
}
