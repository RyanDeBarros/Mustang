#include "ActorTesselation.h"

#include "../transform/Transforms.h"

ActorTesselation2D::ActorTesselation2D(ActorRenderBase2D* actor)
	: m_Actor(actor), m_Transformer()
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(const ActorTesselation2D& other)
	: m_Actor(other.m_Actor), ActorSequencer2D(other), m_Transformer(other.m_Transformer), m_InternalChildren(other.m_InternalChildren)
{
	BindFunctions();
}

ActorTesselation2D::ActorTesselation2D(ActorTesselation2D&& other) noexcept
	: m_Actor(std::move(other.m_Actor)), ActorSequencer2D(std::move(other)), m_Transformer(std::move(other.m_Transformer)), m_InternalChildren(std::move(other.m_InternalChildren))
{
	BindFunctions();
}

ActorTesselation2D& ActorTesselation2D::operator=(const ActorTesselation2D& other)
{
	m_Actor = other.m_Actor;
	m_Transformer = other.m_Transformer;
	m_InternalChildren = other.m_InternalChildren;
	BindFunctions();
	ActorSequencer2D::operator=(other);
	return *this;
}

ActorTesselation2D& ActorTesselation2D::operator=(ActorTesselation2D&& other) noexcept
{
	m_Actor = std::move(other.m_Actor);
	m_Transformer = std::move(other.m_Transformer);
	m_InternalChildren = std::move(other.m_InternalChildren);
	BindFunctions();
	ActorSequencer2D::operator=(std::move(other));
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

//void ActorTesselation2D::PushBackLocal(const Transform2D& local)
//{
//	m_Elements.push_back(PackedTransform2D(local));
//}
//
//void ActorTesselation2D::PushBackLocal(const PackedTransform2D& local)
//{
//	m_Elements.push_back(local);
//}
//
//void ActorTesselation2D::PushBackLocals(const std::vector<Transform2D>& locals)
//{
//	m_Elements.reserve(m_Elements.capacity() + locals.size());
//	for (const auto& local : locals)
//		PushBackLocal(local);
//}
//
//void ActorTesselation2D::PushBackLocals(const std::vector<PackedTransform2D>& locals)
//{
//	m_Elements.reserve(m_Elements.capacity() + locals.size());
//	for (const auto& local : locals)
//		PushBackLocal(local);
//}

ActorPrimitive2D* const ActorTesselation2D::operator[](PrimitiveIndex i)
{
	if (i >= static_cast<int>(PrimitiveCount()) || i < 0)
		return nullptr;
	return f_operator(i);
}

ActorPrimitive2D* const ActorTesselation2D::f_prim_operator(ActorTesselation2D* const tessel, PrimitiveIndex i)
{
	ActorPrimitive2D* const primitive = static_cast<ActorPrimitive2D* const>(tessel->m_Actor);
	//primitive->m_Transformer.self.transform = Transforms::AbsTo(
			//Transforms::AbsTo(
				//tessel->m_ActorPreDrawTransforms[0],
				//tessel->m_Transformer.children[i]->self.transform),
			//tessel->m_Transformer.self.transform);
	// TODO use Transformer instead to not have to sync every frame
	//tessel->m_Elements[i].Sync(tessel->m_Transform.self);
	primitive->m_Transformer.self.Sync(tessel->m_Transformer.children[i]->self);
	primitive->FlagTransform();
	return primitive;
}

ActorPrimitive2D* const ActorTesselation2D::f_sequ_operator(ActorTesselation2D* const tessel, PrimitiveIndex i)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	ActorPrimitive2D* const primitive = (*sequencer)[i % tessel->RenderSeqCount()];
	if (primitive)
	{
		//primitive->m_Transformer.self.transform = Transforms::AbsTo(
				//Transforms::AbsTo(
					//tessel->m_ActorPreDrawTransforms[i % tessel->m_ActorPreDrawTransforms.size()],
					//tessel->m_Transformer.children[i / tessel->RenderSeqCount()]->self.transform)
				//, tessel->m_Transformer.self.transform);
		//primitive->FlagTransform();

		//tessel->m_Elements[i / tessel->RenderSeqCount()].Sync(tessel->m_Transform.self);
		primitive->m_Transformer.self.Sync(tessel->m_Transformer.children[i / tessel->RenderSeqCount()]->self);
		primitive->FlagTransform();
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
	//return static_cast<BufferCounter>(m_Elements.size()) * RenderSeqCount();
	return static_cast<BufferCounter>(m_Transformer.children.size()) * RenderSeqCount();
}

void ActorTesselation2D::OnPreDraw()
{
	f_OnPreDraw();
}

void ActorTesselation2D::f_prim_OnPreDraw(ActorTesselation2D* const tessel)
{
	//tessel->m_ActorPreDrawTransforms[0] = static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get())->m_Transformer.self.transform;
}

void ActorTesselation2D::f_sequ_OnPreDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	sequencer->OnPreDraw();
	//std::vector<Transform2D>().swap(tessel->m_ActorPreDrawTransforms);
	//int i = 0;
	//while (ActorPrimitive2D* const prim = (*sequencer)[i++])
		//tessel->m_ActorPreDrawTransforms.push_back(prim->m_Transformer.self.transform);
}

void ActorTesselation2D::OnPostDraw()
{
	f_OnPostDraw();
}

void ActorTesselation2D::f_prim_OnPostDraw(ActorTesselation2D* const tessel)
{
	//static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get())->m_Transformer.self.transform = tessel->m_ActorPreDrawTransforms[0];
	// TODO is sync necessary?
	//static_cast<ActorPrimitive2D* const>(tessel->m_Actor.get())->m_Transformer.Sync();
}

void ActorTesselation2D::f_sequ_OnPostDraw(ActorTesselation2D* const tessel)
{
	ActorSequencer2D* const sequencer = static_cast<ActorSequencer2D* const>(tessel->m_Actor);
	//for (auto i = 0; i < tessel->m_ActorPreDrawTransforms.size(); i++)
	//{
		//(*sequencer)[i]->m_Transformer.self.transform = tessel->m_ActorPreDrawTransforms[i];
		// TODO necessary sync call?
		//(*sequencer)[i]->m_Transformer.Sync();
	//}
	sequencer->OnPostDraw();
}
