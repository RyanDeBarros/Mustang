#include "ActorTesselation.h"

#include "../transform/Transforms.h"
#include "render/CanvasLayer.h"

ActorTransformTesselation2D::ActorTransformTesselation2D(TransformableActor2D* actor)
	: m_Actor(actor)
{
}

void ActorTransformTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
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

void ActorTransformTesselation2D::PushBackStatic(const Transform2D& transform, bool sync_on_attach)
{
	auto transformer = std::make_unique<Transformer2D>(transform);
	m_Transformer.Attach(transformer.get());
	if (sync_on_attach)
		transformer->Sync();
	m_InternalChildren.push_back(std::move(transformer));
}

void ActorTransformTesselation2D::PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach)
{
	m_InternalChildren.reserve(m_InternalChildren.capacity() + transforms.size());
	for (const auto& transform : transforms)
		PushBackStatic(transform, sync_on_attach);
}

ActorModulateTesselation2D::ActorModulateTesselation2D(ModulatableActor2D* actor)
	: m_Actor(actor)
{
}

void ActorModulateTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
{
	Modulator* parent = m_Actor->RefModulator()->parent;
	for (const auto& mo : m_Modulator.children)
	{
		m_Actor->RefModulator()->parent = mo;
		m_Actor->RefModulator()->Sync();
		m_Actor->RequestDraw(canvas_layer);
	}
	m_Actor->RefModulator()->parent = parent;
}

void ActorModulateTesselation2D::PushBackStatic(const Modulate& modulate, bool sync_on_attach)
{
	auto modulator = std::make_unique<Modulator>(modulate);
	m_Modulator.Attach(modulator.get());
	if (sync_on_attach)
		modulator->Sync();
	m_InternalChildren.push_back(std::move(modulator));
}

void ActorModulateTesselation2D::PushBackStatic(const std::vector<Modulate>& modulates, bool sync_on_attach)
{
	m_InternalChildren.reserve(m_InternalChildren.capacity() + modulates.size());
	for (const auto& modulate : modulates)
		PushBackStatic(modulate, sync_on_attach);
}

ActorTesselation2D::ActorTesselation2D(ProteanActor2D* actor)
	: m_Actor(actor)
{
}

void ActorTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
{
	ProteanLinker2D* parent = m_Actor->RefProteanLinker()->parent;
	for (const auto& pr : m_ProteanLinker.children)
	{
		m_Actor->RefProteanLinker()->parent = pr;
		m_Actor->RefProteanLinker()->Sync();
		m_Actor->RequestDraw(canvas_layer);
	}
	m_Actor->RefProteanLinker()->parent = parent;
}

void ActorTesselation2D::PushBackStatic(const Proteate2D& proteate, bool sync_on_attach)
{
	auto linker = std::make_unique<ProteanLinker2D>(proteate);
	m_ProteanLinker.Attach(linker.get());
	if (sync_on_attach)
		linker->Sync();
	m_InternalChildren.push_back(std::move(linker));
}

void ActorTesselation2D::PushBackStatic(const std::vector<Proteate2D>& proteates, bool sync_on_attach)
{
	m_InternalChildren.reserve(m_InternalChildren.capacity() + proteates.size());
	for (const auto& proteate : proteates)
		PushBackStatic(proteate, sync_on_attach);
}
