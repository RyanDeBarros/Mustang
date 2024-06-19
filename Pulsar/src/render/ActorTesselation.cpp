#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D_P actor)
	: m_Actor(actor)
{
}

ActorTesselation2D::~ActorTesselation2D()
{
}

void ActorTesselation2D::Insert(const TesselRect& rect)
{
	m_RectVector.push_back(rect);
}

ActorPrimitive2D* ActorTesselation2D::operator[](const int& i)
{
	if (i >= PrimitiveCount())
		return nullptr;
	if (m_Actor.index() == 0)
	{
		const TesselRect& rect = m_RectVector[i];
		auto& primitive = std::get<ActorPrimitive2D* const>(m_Actor);
		primitive->SetPosition((float)rect.x, (float)rect.y);
		primitive->SetRotation((float)rect.r);
		primitive->SetScale((float)rect.w, (float)rect.h);
		return primitive;
	}
	else
	{
		const TesselRect& rect = m_RectVector[i / std::get<ActorSequencer2D* const>(m_Actor)->PrimitiveCount()];
		auto const& primitive = std::get<ActorSequencer2D* const>(m_Actor)
				->operator[](i % std::get<ActorSequencer2D* const>(m_Actor)->PrimitiveCount());
		// TODO transform should be added onto the existing transform temporarily. This will be done by having two transforms in a primitive: 
		primitive->SetPosition((float)rect.x, (float)rect.y);
		primitive->SetRotation((float)rect.r);
		primitive->SetScale((float)rect.w, (float)rect.h);
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
		return m_RectVector.size();
	else
		return m_RectVector.size() * std::get<ActorSequencer2D* const>(m_Actor)->PrimitiveCount();
}
