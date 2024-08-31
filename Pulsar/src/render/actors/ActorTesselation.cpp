#include "ActorTesselation.h"

#include "render/CanvasLayer.h"
#include "Logger.inl"

template<int _TesselFickle, int _ActorFickle>
static void request_draw(CanvasLayer* layer, ActorTesselation2D* tessel, FickleActor2D* actor)
{
	if constexpr (_TesselFickle == FickleType::Protean)
	{
		if constexpr (_ActorFickle == FickleType::Protean)
		{
			auto tessel_children = tessel->Fickler().ProteanChildren();
			auto actor_linker = actor->Fickler().ProteanLinker();
			for (size_t i = 0; i < tessel_children.size(); i++)
			{
				actor_linker.SyncAll(tessel_children, i);
				actor_linker.SyncChildrenAll();
				actor->RequestDraw(layer);
			}
		}
		else if constexpr (_ActorFickle == FickleType::Transformable)
		{
			auto tessel_children = tessel->Fickler().ProteanChildren();
			auto actor_transformer = actor->Fickler().Transformer();
			for (size_t i = 0; i < tessel_children.size(); i++)
			{
				actor_transformer->self.Sync(tessel_children.at<Transformer2D>(i)->self);
				actor_transformer->SyncChildren();
				actor->RequestDraw(layer);
			}
		}
		else if constexpr (_ActorFickle == FickleType::Modulatable)
		{
			auto tessel_children = tessel->Fickler().ProteanChildren();
			auto actor_modulator = actor->Fickler().Modulator();
			for (size_t i = 0; i < tessel_children.size(); i++)
			{
				actor_modulator->self.Sync(tessel_children.at<Modulator>(i)->self);
				actor_modulator->SyncChildren();
				actor->RequestDraw(layer);
			}
		}
	}
	else if constexpr (_TesselFickle == FickleType::Transformable)
	{
		if constexpr (_ActorFickle == FickleType::Protean || _ActorFickle == FickleType::Transformable)
		{
			auto tessel_transformer = tessel->Fickler().Transformer();
			auto actor_transformer = actor->Fickler().Transformer();
			for (size_t i = 0; i < tessel_transformer->children.size(); i++)
			{
				actor_transformer->self.Sync(tessel_transformer->children[i]->self);
				actor_transformer->SyncChildren();
				actor->RequestDraw(layer);
			}
		}
	}
	else if constexpr (_TesselFickle == FickleType::Modulatable)
	{
		if (_ActorFickle == FickleType::Protean || _ActorFickle == FickleType::Modulatable)
		{
			auto tessel_modulator = tessel->Fickler().Modulator();
			auto actor_modulator = actor->Fickler().Modulator();
			for (size_t i = 0; i < tessel_modulator->children.size(); i++)
			{
				actor_modulator->self.Sync(tessel_modulator->children[i]->self);
				actor_modulator->SyncChildren();
				actor->RequestDraw(layer);
			}
		}
	}
}

ActorTesselation2D::ActorTesselation2D(FickleActor2D* actor, FickleType fickle_type)
	: FickleActor2D(fickle_type), m_Actor(actor)
{
	if (!actor)
		Logger::LogErrorFatal("Null actor in tesselation.");
	switch (fickle_type)
	{
	case FickleType::Protean:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &request_draw<FickleType::Protean, FickleType::Protean>;
			break;
		case FickleType::Transformable:
			f_RequestDraw = &request_draw<FickleType::Protean, FickleType::Transformable>;
			break;
		case FickleType::Modulatable:
			f_RequestDraw = &request_draw<FickleType::Protean, FickleType::Modulatable>;
			break;
		}
		break;
	[[likely]] case FickleType::Transformable:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &request_draw<FickleType::Transformable, FickleType::Protean>;
			break;
		case FickleType::Transformable:
			f_RequestDraw = &request_draw<FickleType::Transformable, FickleType::Transformable>;
			break;
		case FickleType::Modulatable:
			f_RequestDraw = &request_draw<FickleType::Transformable, FickleType::Modulatable>;
			break;
		}
		break;
	[[unlikely]] case FickleType::Modulatable:
		switch (actor->Fickler().Type())
		{
		case FickleType::Protean:
			f_RequestDraw = &request_draw<FickleType::Modulatable, FickleType::Protean>;
			break;
		case FickleType::Transformable:
			f_RequestDraw = &request_draw<FickleType::Modulatable, FickleType::Transformable>;
			break;
		case FickleType::Modulatable:
			f_RequestDraw = &request_draw<FickleType::Modulatable, FickleType::Modulatable>;
			break;
		}
		break;
	}
}

ActorTesselation2D::~ActorTesselation2D()
{
	for (auto ptr : m_InternalPlacements)
		delete ptr;
}

void ActorTesselation2D::RequestDraw(CanvasLayer* canvas_layer)
{
	f_RequestDraw(canvas_layer, this, m_Actor);
}

void ActorTesselation2D::PushBackStatic(const Transform2D& transform, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Transformable)
		return;
	auto new_fickler = new Fickler2D(FickleType::Transformable);
	new_fickler->transformable->self.transform = transform;
	if (sync_on_attach)
	{
		m_Fickler.AttachTransformer(new_fickler->Transformer());
		new_fickler->Transformer()->Sync();
	}
	else
	{
		new_fickler->Transformer()->Sync();
		m_Fickler.AttachTransformer(new_fickler->Transformer());
	}
	m_InternalPlacements.push_back(new_fickler);
}

void ActorTesselation2D::PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Transformable)
		return;
	m_InternalPlacements.reserve(m_InternalPlacements.capacity() + transforms.size());
	for (const auto& transform : transforms)
		PushBackStatic(transform, sync_on_attach);
}

void ActorTesselation2D::PushBackStatic(const Transform2D& transform, const Modulate& modulate, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Protean)
		return;
	auto new_fickler = new Fickler2D(FickleType::Protean);
	new_fickler->transformable->self.transform = transform;
	new_fickler->modulatable->self.modulate = modulate;
	if (sync_on_attach)
	{
		m_Fickler.Attach(*new_fickler);
		new_fickler->SyncAll();
	}
	else
	{
		new_fickler->SyncAll();
		m_Fickler.Attach(*new_fickler);
	}
	m_InternalPlacements.push_back(new_fickler);
}

void ActorTesselation2D::PushBackStatic(const std::vector<std::pair<Transform2D, Modulate>>& proteates, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Protean)
		return;
	m_InternalPlacements.reserve(m_InternalPlacements.capacity() + proteates.size());
	for (const auto& proteate : proteates)
		PushBackStatic(proteate.first, proteate.second, sync_on_attach);
}

void ActorTesselation2D::PushBackStatic(const Modulate& modulate, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Modulatable)
		return;
	auto new_fickler = new Fickler2D(FickleType::Modulatable);
	new_fickler->modulatable->self.modulate = modulate;
	if (sync_on_attach)
	{
		m_Fickler.AttachModulator(new_fickler->Modulator());
		new_fickler->Modulator()->Sync();
	}
	else
	{
		new_fickler->Modulator()->Sync();
		m_Fickler.AttachModulator(new_fickler->Modulator());
	}
	m_InternalPlacements.push_back(new_fickler);
}

void ActorTesselation2D::PushBackStatic(const std::vector<Modulate>& modulates, bool sync_on_attach)
{
	if (m_Fickler.Type() != FickleType::Modulatable)
		return;
	m_InternalPlacements.reserve(m_InternalPlacements.capacity() + modulates.size());
	for (const auto& modulate : modulates)
		PushBackStatic(modulate, sync_on_attach);
}
