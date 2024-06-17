#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorPrimitive2D actor)
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
	if (i >= m_RectVector.size())
		return nullptr;
	const TesselRect& rect = m_RectVector[i];
	m_Actor.SetPosition((float)rect.x, (float)rect.y);
	m_Actor.SetRotation((float)rect.r);
	m_Actor.SetScale((float)rect.w, (float)rect.h);
	return &m_Actor;
}
