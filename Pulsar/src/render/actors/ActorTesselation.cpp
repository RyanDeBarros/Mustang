#include "ActorTesselation.h"

#include "render/CanvasLayer.h"
#include "Logger.inl"

static void protean_request_draw_for_protean(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	ProteanLinker2D* linker = actor->Fickler().ProteanLinker();
	for (const auto& pr : tessel->Fickler().ProteanLinker()->children)
	{
		linker->self.Sync(pr->self);
		linker->Sync();
		actor->RequestDraw(layer);
	}
}

static void protean_request_draw_for_transformable(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	Transformer2D* transformer = actor->Fickler().Transformer();
	for (const auto& pr : tessel->Fickler().ProteanLinker()->children)
	{
		transformer->self.Sync(pr->self.packedP, pr->self.packedRS);
		transformer->SyncChildren();
		actor->RequestDraw(layer);
	}
}

static void protean_request_draw_for_modulatable(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	Modulator* modulator = actor->Fickler().Modulator();
	for (const auto& pr : tessel->Fickler().ProteanLinker()->children)
	{
		modulator->self.Sync(pr->self.packedM);
		modulator->SyncChildren();
		actor->RequestDraw(layer);
	}
}

static void transformable_request_draw_for_protean(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	ProteanLinker2D* linker = actor->Fickler().ProteanLinker();
	for (const auto& tr : tessel->Fickler().Transformer()->children)
	{
		linker->self.SyncT(tr->self);
		linker->SyncChildren();
		actor->RequestDraw(layer);
	}
}

static void transformable_request_draw_for_transformable(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	Transformer2D* transformer = actor->Fickler().Transformer();
	for (const auto& tr : tessel->Fickler().Transformer()->children)
	{
		transformer->self.Sync(tr->self);
		transformer->SyncChildren();
		actor->RequestDraw(layer);
	}
}

static void modulatable_request_draw_for_protean(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	ProteanLinker2D* linker = actor->Fickler().ProteanLinker();
	for (const auto& mo : tessel->Fickler().Modulator()->children)
	{
		linker->self.SyncM(mo->self);
		linker->Sync();
		actor->RequestDraw(layer);
	}
}

static void modulatable_request_draw_for_modulatable(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	Modulator* modulator = actor->Fickler().Modulator();
	for (const auto& mo : tessel->Fickler().Modulator()->children)
	{
		modulator->self.Sync(mo->self);
		modulator->SyncChildren();
		actor->RequestDraw(layer);
	}
}

ActorTesselation2D::ActorTesselation2D(FickleActor2D* actor, FickleType fickle_type)
	: FickleActor2D(fickle_type), m_Actor(actor), m_InternalPlacements(fickle_type)
{
	if (!actor)
		Logger::LogErrorFatal("Null actor in tesselation.");
	switch (fickle_type)
	{
	case FickleType::Protean:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &protean_request_draw_for_protean;
			break;
		[[likely]] case FickleType::Transformable:
			f_RequestDraw = &protean_request_draw_for_transformable;
			break;
		[[unlikely]] case FickleType::Modulatable:
			f_RequestDraw = &protean_request_draw_for_modulatable;
			break;
		}
		break;
	[[likely]] case FickleType::Transformable:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &transformable_request_draw_for_protean;
			break;
		[[likely]] case FickleType::Transformable:
			f_RequestDraw = &transformable_request_draw_for_transformable;
			break;
		[[unlikely]] case FickleType::Modulatable:
			throw BadFickleSelect(fickle_type, m_Actor->Fickler().Type());
			break;
		}
		break;
	[[unlikely]] case FickleType::Modulatable:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &modulatable_request_draw_for_protean;
			break;
		[[likely]] case FickleType::Transformable:
			throw BadFickleSelect(fickle_type, m_Actor->Fickler().Type());
			break;
		[[unlikely]] case FickleType::Modulatable:
			f_RequestDraw = &modulatable_request_draw_for_modulatable;
			break;
		}
		break;
	}
}

void ActorTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
{
	f_RequestDraw(canvas_layer, this, m_Actor);
}

void ActorTesselation2D::PushBackStatic(const Transform2D& transform, bool sync_on_attach)
{
	if (!m_Fickler.IsTransformable())
		return;
	auto transformer = std::make_unique<Transformer2D>(transform);
	m_Fickler.Transformer()->Attach(transformer.get());
	if (sync_on_attach)
		transformer->Sync();
	m_InternalPlacements.Transformers()->push_back(std::move(transformer));
}

void ActorTesselation2D::PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach)
{
	if (!m_Fickler.IsTransformable())
		return;
	m_InternalPlacements.Transformers()->reserve(m_InternalPlacements.Transformers()->capacity() + transforms.size());
	for (const auto& transform : transforms)
		PushBackStatic(transform, sync_on_attach);
}

void ActorTesselation2D::PushBackStatic(const Proteate2D& proteate, bool sync_on_attach)
{
	if (!m_Fickler.IsProtean())
		return;
	auto linker = std::make_unique<ProteanLinker2D>(proteate);
	m_Fickler.ProteanLinker()->Attach(linker.get());
	if (sync_on_attach)
		linker->Sync();
	m_InternalPlacements.ProteanLinkers()->push_back(std::move(linker));
}

void ActorTesselation2D::PushBackStatic(const std::vector<Proteate2D>& proteates, bool sync_on_attach)
{
	if (!m_Fickler.IsProtean())
		return;
	m_InternalPlacements.ProteanLinkers()->reserve(m_InternalPlacements.ProteanLinkers()->capacity() + proteates.size());
	for (const auto& proteate : proteates)
		PushBackStatic(proteate, sync_on_attach);
}

void ActorTesselation2D::PushBackStatic(const Modulate& modulate, bool sync_on_attach)
{
	if (!m_Fickler.IsModulatable())
		return;
	auto modulator = std::make_unique<Modulator>(modulate);
	m_Fickler.Modulator()->Attach(modulator.get());
	if (sync_on_attach)
		modulator->Sync();
	m_InternalPlacements.Modulators()->push_back(std::move(modulator));
}

void ActorTesselation2D::PushBackStatic(const std::vector<Modulate>& modulates, bool sync_on_attach)
{
	if (!m_Fickler.IsModulatable())
		return;
	m_InternalPlacements.Modulators()->reserve(m_InternalPlacements.Modulators()->capacity() + modulates.size());
	for (const auto& modulate : modulates)
		PushBackStatic(modulate, sync_on_attach);
}
