#include "ActorTesselation.h"

ActorTesselation2D::ActorTesselation2D(const std::shared_ptr<ActorRenderBase2D>& actor)
	: m_Actor(actor), m_Transform(std::make_shared<TransformableProxy2D>()), m_Transformer(m_Transform)
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(std::shared_ptr<ActorRenderBase2D>&& actor)
	: m_Actor(std::move(actor)), m_Transform(std::make_shared<TransformableProxy2D>()), m_Transformer(m_Transform)
{
	BindFunctions();
}

//ActorTesselation2D::ActorTesselation2D(const ActorTesselation2D& other)
//	: m_Actor(other.m_Actor), ActorSequencer2D(other), TransformableProxy2D(other), m_Transformer(other.m_Transformer), m_TransformsList(other.m_TransformsList)
//{
//	BindFunctions();
//}

ActorTesselation2D::ActorTesselation2D(ActorTesselation2D&& other) noexcept
	: m_Actor(std::move(other.m_Actor)), ActorSequencer2D(std::move(other)), m_Transform(std::move(other.m_Transform)), m_Transformer(std::move(other.m_Transformer)), m_TransformsList(std::move(other.m_TransformsList))
{
	BindFunctions();
}

//ActorTesselation2D& ActorTesselation2D::operator=(const ActorTesselation2D& other)
//{
//	m_Actor = other.m_Actor;
//	m_Transformer = other.m_Transformer;
//	m_TransformsList = other.m_TransformsList;
//	BindFunctions();
//	ActorSequencer2D::operator=(other);
//	TransformableProxy2D::operator=(other);
//	return *this;
//}

ActorTesselation2D& ActorTesselation2D::operator=(ActorTesselation2D&& other) noexcept
{
	m_Actor = std::move(other.m_Actor);
	m_Transform = std::move(other.m_Transform);
	m_Transformer = std::move(other.m_Transformer);
	m_TransformsList = std::move(other.m_TransformsList);
	BindFunctions();
	ActorSequencer2D::operator=(std::move(other));
	return *this;
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

void ActorTesselation2D::PushBackGlobal(const Transform2D& child)
{
	std::shared_ptr<TransformableProxy2D> addition(std::make_shared<TransformableProxy2D>(child));
	m_Transformer.PushBackGlobal(addition, false);
	m_TransformsList.push_back(addition);
}

void ActorTesselation2D::PushBackGlobals(const std::vector<Transform2D>& children)
{
	std::vector<std::shared_ptr<TransformableProxy2D>> additions;
	for (const auto& child : children)
	{
		std::shared_ptr<TransformableProxy2D> addition(std::make_shared<TransformableProxy2D>(child));
		m_Transformer.PushBackGlobal(addition, false);
		additions.push_back(addition);
	}
}

void ActorTesselation2D::PushBackLocal(const Transform2D& local)
{
	m_TransformsList.push_back(m_Transformer.PushBackLocal(local, false));
}

void ActorTesselation2D::PushBackLocals(const std::vector<Transform2D>& locals)
{
	std::vector<std::shared_ptr<TransformableProxy2D>> additions(m_Transformer.PushBackLocals(locals, false));
	m_TransformsList.insert(m_TransformsList.end(), additions.begin(), additions.end());
}

ActorPrimitive2D* const ActorTesselation2D::operator[](int i)
{
	if (i >= static_cast<int>(PrimitiveCount()) || i < 0)
		return nullptr;
	return f_operator(i);
}

ActorPrimitive2D* const ActorTesselation2D::f_prim_operator(ActorTesselation2D* const tessel, int i)
{
	ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get());
	primitive->SetTransform(Transform::AbsTo(Transform::AbsTo(tessel->m_ActorPreDrawTransforms[0], tessel->m_Transformer.GetLocalTransform(i)), tessel->m_Transform->GetTransform()));
	return primitive;
}

ActorPrimitive2D* const ActorTesselation2D::f_sequ_operator(ActorTesselation2D* const tessel, int i)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor.get());
	ActorPrimitive2D* const primitive = (*sequencer)[i % tessel->RenderSeqCount()];
	if (primitive)
		primitive->SetTransform(Transform::AbsTo(Transform::AbsTo(tessel->m_ActorPreDrawTransforms[i % tessel->m_ActorPreDrawTransforms.size()], tessel->m_Transformer.GetLocalTransform(i / tessel->RenderSeqCount())), tessel->m_Transform->GetTransform()));
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

void ActorTesselation2D::SetZIndex(ZIndex z)
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
