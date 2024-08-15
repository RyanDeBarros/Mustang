#include "ActorTesselation.h"

#include "../transform/Transforms.h"
#include "render/CanvasLayer.h"

ActorTesselation2D::ActorTesselation2D(ProteanActor2D* actor)
	: m_Actor(actor), ProteanActor2D()
{
}

ActorTesselation2D::ActorTesselation2D(const ActorTesselation2D& other)
	: m_Actor(other.m_Actor), ProteanActor2D(other), m_InternalChildren(other.m_InternalChildren)
{
}

ActorTesselation2D::ActorTesselation2D(ActorTesselation2D&& other) noexcept
	: m_Actor(std::move(other.m_Actor)), ProteanActor2D(std::move(other)), m_InternalChildren(std::move(other.m_InternalChildren))
{
}

ActorTesselation2D& ActorTesselation2D::operator=(const ActorTesselation2D& other)
{
	ProteanActor2D::operator=(other);
	m_Actor = other.m_Actor;
	m_InternalChildren = other.m_InternalChildren;
	return *this;
}

ActorTesselation2D& ActorTesselation2D::operator=(ActorTesselation2D&& other) noexcept
{
	ProteanActor2D::operator=(std::move(other));
	m_Actor = std::move(other.m_Actor);
	m_InternalChildren = std::move(other.m_InternalChildren);
	return *this;
}

void ActorTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
{
	Transformer2D* parent = m_Actor->RefTransformer()->parent;
	for (const auto& tr : m_Transformer.children)
	{
		m_Actor->RefTransformer()->parent = tr;
		m_Actor->RefTransformer()->Sync();
		m_Actor->RequestDraw(canvas_layer);
	}
	m_Actor->RefTransformer()->parent = parent;
}

void ActorTesselation2D::PushBackStatic(const Transform2D& transform, bool sync_on_attach)
{
	auto transformer = std::make_shared<Transformer2D>(transform);
	m_InternalChildren.push_back(transformer);
	m_Transformer.Attach(transformer.get());
	if (sync_on_attach)
		transformer->Sync();
}

void ActorTesselation2D::PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach)
{
	m_InternalChildren.reserve(m_InternalChildren.capacity() + transforms.size());
	for (const auto& transform : transforms)
		PushBackStatic(transform, sync_on_attach);
}
