#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D* const actor)
	: m_Actor(actor), ActorSequencer2D(0)
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
		f_OnPreDraw = [](){};
		f_OnPostDraw = [](){};
	}
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
	return f_operator(i);
}

ActorPrimitive2D* const ActorTesselation2D::f_prim_operator(ActorTesselation2D const* const tessel, const int& i)
{
	ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(tessel->m_Actor);
	primitive->SetTransform(tessel->m_GlobalTransform ^ tessel->m_RectVector[i / tessel->RenderSeqCount()] ^ tessel->m_ActorOffsets[i % tessel->RenderSeqCount()]);
	return primitive;
}

ActorPrimitive2D* const ActorTesselation2D::f_sequ_operator(ActorTesselation2D const* const tessel, const int& i)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	ActorPrimitive2D* const primitive = (*sequencer)[i % tessel->RenderSeqCount()];
	if (primitive)
		primitive->SetTransform(tessel->m_GlobalTransform ^ tessel->m_RectVector[i / tessel->RenderSeqCount()] ^ tessel->m_ActorOffsets[i % tessel->RenderSeqCount()]);
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
	f_OnPreDraw();
}

void ActorTesselation2D::f_prim_OnPreDraw(ActorTesselation2D* const tessel)
{
	tessel->m_ActorOffsets[0] = static_cast<ActorPrimitive2D* const>(tessel->m_Actor)->GetTransform();
}

void ActorTesselation2D::f_sequ_OnPreDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	sequencer->OnPreDraw();
	for (int i = 0; i < tessel->m_ActorOffsets.size(); i++)
		tessel->m_ActorOffsets[i] = (*sequencer)[i]->GetTransform();
}

void ActorTesselation2D::OnPostDraw()
{
	f_OnPostDraw();
}

void ActorTesselation2D::f_prim_OnPostDraw(ActorTesselation2D* const tessel)
{
	static_cast<ActorPrimitive2D* const>(tessel->m_Actor)->SetTransform(tessel->m_ActorOffsets[0]);
}

void ActorTesselation2D::f_sequ_OnPostDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	for (int i = 0; i < tessel->m_ActorOffsets.size(); i++)
		(*sequencer)[i]->SetTransform(tessel->m_ActorOffsets[i]);
	sequencer->OnPostDraw();
}
