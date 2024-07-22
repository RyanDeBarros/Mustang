#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(const std::shared_ptr<ActorRenderBase2D>& actor)
	: m_Actor(actor), ActorSequencer2D(0), Transformable2D(Transform2D{}), m_Transformer(m_Transform, {})
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(const ActorTesselation2D& other)
	: m_Actor(other.m_Actor), ActorSequencer2D(other.GetZIndex()), Transformable2D(other.m_Transform), m_Transformer(other.m_Transformer)
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(ActorTesselation2D&& other) noexcept
	: m_Actor(other.m_Actor), ActorSequencer2D(other.GetZIndex()), Transformable2D(other.m_Transform), m_Transformer(other.m_Transformer)
{
	BindFunctions();
}

void ActorTesselation2D::BindFunctions()
{
	if (dynamic_cast<ActorPrimitive2D*>(m_Actor.get()))
	{
		f_operator = std::bind(&ActorTesselation2D::f_prim_operator, this, std::placeholders::_1);
		f_RenderSeqCount = std::bind(&ActorTesselation2D::f_prim_RenderSeqCount, this);
		f_OnPreDraw = std::bind(&ActorTesselation2D::f_prim_OnPreDraw, this);
		f_OnPostDraw = std::bind(&ActorTesselation2D::f_prim_OnPostDraw, this);
		m_ActorPreDrawTransforms.push_back({});
	}
	else if (dynamic_cast<ActorSequencer2D*>(m_Actor.get()))
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

void ActorTesselation2D::PushBack(const std::shared_ptr<Transformable2D>& child)
{
	m_Transformer.PushBack(child, false);
}

void ActorTesselation2D::PushBack(const std::vector<std::shared_ptr<Transformable2D>>& children)
{
	m_Transformer.PushBack(children, false);
}

ActorPrimitive2D* const ActorTesselation2D::operator[](const int& i)
{
	if (i >= static_cast<int>(PrimitiveCount()) || i < 0)
		return nullptr;
	return f_operator(i);
}

ActorPrimitive2D* const ActorTesselation2D::f_prim_operator(ActorTesselation2D* const tessel, const int& i)
{
	ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get());
	primitive->SetTransform(*tessel->m_Transform ^ tessel->m_Transformer.GetLocalTransform(i) ^ tessel->m_ActorPreDrawTransforms[0]);
	return primitive;
}

ActorPrimitive2D* const ActorTesselation2D::f_sequ_operator(ActorTesselation2D* const tessel, const int& i)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor.get());
	ActorPrimitive2D* const primitive = (*sequencer)[i % tessel->RenderSeqCount()];
	if (primitive)
		primitive->SetTransform(*tessel->m_Transform ^ tessel->m_Transformer.GetLocalTransform(i / tessel->RenderSeqCount()) ^ tessel->m_ActorPreDrawTransforms[i % tessel->m_ActorPreDrawTransforms.size()]);
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
	return static_cast<ActorSequencer2D* const>(tessel->m_Actor.get())->PrimitiveCount();
}

ZIndex ActorTesselation2D::GetZIndex() const
{
	return m_Actor->GetZIndex();
}

void ActorTesselation2D::SetZIndex(const ZIndex& z)
{
	m_Actor->SetZIndex(z);
}

BufferCounter ActorTesselation2D::PrimitiveCount() const
{
	return static_cast<BufferCounter>(m_Transformer.Size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	f_OnPreDraw();
}

void ActorTesselation2D::f_prim_OnPreDraw(ActorTesselation2D* const tessel)
{
	tessel->m_ActorPreDrawTransforms[0] = static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get())->GetTransform();
}

void ActorTesselation2D::f_sequ_OnPreDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor.get());
	sequencer->OnPreDraw();
	std::vector<Transform2D>().swap(tessel->m_ActorPreDrawTransforms);
	int i = 0;
	while (ActorPrimitive2D* const prim = (*sequencer)[i++])
		tessel->m_ActorPreDrawTransforms.push_back(prim->GetTransform());
}

void ActorTesselation2D::OnPostDraw()
{
	f_OnPostDraw();
}

void ActorTesselation2D::f_prim_OnPostDraw(ActorTesselation2D* const tessel)
{
	static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get())->SetTransform(tessel->m_ActorPreDrawTransforms[0]);
}

void ActorTesselation2D::f_sequ_OnPostDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor.get());
	for (auto i = 0; i < tessel->m_ActorPreDrawTransforms.size(); i++)
		(*sequencer)[i]->SetTransform(tessel->m_ActorPreDrawTransforms[i]);
	sequencer->OnPostDraw();
}
