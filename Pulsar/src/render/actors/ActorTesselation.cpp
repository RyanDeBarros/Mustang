#include "ActorTesselation.h"

#include "../transform/Transforms.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D* actor)
	: m_Actor(actor), ActorSequencer2D()
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(const ActorTesselation2D& other)
	: m_Actor(other.m_Actor), ActorSequencer2D(other), m_InternalChildren(other.m_InternalChildren)
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(ActorTesselation2D&& other) noexcept
	: m_Actor(std::move(other.m_Actor)), ActorSequencer2D(std::move(other)), m_InternalChildren(std::move(other.m_InternalChildren))
{
	BindFunctions();
}

ActorTesselation2D& ActorTesselation2D::operator=(const ActorTesselation2D& other)
{
	ActorSequencer2D::operator=(other);
	m_Actor = other.m_Actor;
	m_InternalChildren = other.m_InternalChildren;
	BindFunctions();
	return *this;
}

ActorTesselation2D& ActorTesselation2D::operator=(ActorTesselation2D&& other) noexcept
{
	ActorSequencer2D::operator=(std::move(other));
	m_Actor = std::move(other.m_Actor);
	m_InternalChildren = std::move(other.m_InternalChildren);
	BindFunctions();
	return *this;
}

void ActorTesselation2D::BindFunctions()
{
	if (dynamic_cast<ActorPrimitive2D*>(m_Actor))
	{
		f_operator = std::bind(&ActorTesselation2D::f_prim_operator, this, std::placeholders::_1);
		f_RenderSeqCount = std::bind(&ActorTesselation2D::f_prim_RenderSeqCount, this);
		f_OnPreDraw = std::bind(&ActorTesselation2D::f_prim_OnPreDraw, this);
		f_OnPostDraw = std::bind(&ActorTesselation2D::f_prim_OnPostDraw, this);
	}
	else if (dynamic_cast<ActorSequencer2D*>(m_Actor))
	{
		f_operator = std::bind(&ActorTesselation2D::f_sequ_operator, this, std::placeholders::_1);
		f_RenderSeqCount = std::bind(&ActorTesselation2D::f_sequ_RenderSeqCount, this);
		f_OnPreDraw = std::bind(&ActorTesselation2D::f_sequ_OnPreDraw, this);
		f_OnPostDraw = std::bind(&ActorTesselation2D::f_sequ_OnPostDraw, this);
	}
	else
	{
		f_operator = [](const int& i) { return nullptr; };
		f_RenderSeqCount = []() { return 0; };
		f_OnPreDraw = []() {};
		f_OnPostDraw = []() {};
	}
}

ActorTesselation2D::~ActorTesselation2D()
{
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

ActorPrimitive2D* const ActorTesselation2D::operator[](PrimitiveIndex i)
{
	if (i >= static_cast<int>(PrimitiveCount()) || i < 0)
		return nullptr;
	return f_operator(i);
}

// TODO instead of assuming primitive or sequencer, let m_Actor be a Transformable, i.e. a class that defines the Transformer() method.
ActorPrimitive2D* const ActorTesselation2D::f_prim_operator(ActorTesselation2D* const tessel, PrimitiveIndex i)
{
	ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(tessel->m_Actor);
	primitive->m_Transformer.self.Sync(tessel->m_Transformer.children[i]->self);
	if (primitive)
	{
		const auto& notify = primitive->m_Transformer.notify;
		if (notify)
			notify->Notify();
	}
	return primitive;
}

ActorPrimitive2D* const ActorTesselation2D::f_sequ_operator(ActorTesselation2D* const tessel, PrimitiveIndex i)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	sequencer->Transformer()->children[i % tessel->RenderSeqCount()]->self.Sync(tessel->m_Transformer.children[i / tessel->RenderSeqCount()]->self);
	ActorPrimitive2D* const primitive = (*sequencer)[i % tessel->RenderSeqCount()];
	if (primitive)
	{
		const auto& notify = primitive->m_Transformer.notify;
		if (notify)
			notify->Notify();
	}
	return primitive;
}

BufferCounter ActorTesselation2D::RenderSeqCount() const
{
	return f_RenderSeqCount();
}

BufferCounter ActorTesselation2D::f_prim_RenderSeqCount(ActorTesselation2D const* const tessel)
{
	return 1;
}

BufferCounter ActorTesselation2D::f_sequ_RenderSeqCount(ActorTesselation2D const* const tessel)
{
	return static_cast<ActorSequencer2D* const>(tessel->m_Actor)->PrimitiveCount();
}

ZIndex ActorTesselation2D::GetZIndex() const
{
	return m_Actor->GetZIndex();
}

void ActorTesselation2D::SetZIndex(ZIndex z)
{
	m_Actor->SetZIndex(z);
}

BufferCounter ActorTesselation2D::PrimitiveCount() const
{
	return static_cast<BufferCounter>(m_Transformer.children.size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	f_OnPreDraw();
}

void ActorTesselation2D::f_prim_OnPreDraw(ActorTesselation2D* const tessel)
{
}

void ActorTesselation2D::f_sequ_OnPreDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	sequencer->OnPreDraw();
}

void ActorTesselation2D::OnPostDraw()
{
	f_OnPostDraw();
}

void ActorTesselation2D::f_prim_OnPostDraw(ActorTesselation2D* const tessel)
{
}

void ActorTesselation2D::f_sequ_OnPostDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	sequencer->OnPostDraw();
}
