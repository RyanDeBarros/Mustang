#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "../ActorRenderBase.h"

class ActorTransformTesselation2D : public TransformableActor2D
{
	TransformableActor2D* m_Actor;
	std::vector<std::unique_ptr<Transformer2D>> m_InternalChildren;

public:
	ActorTransformTesselation2D(TransformableActor2D* actor);

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline ActorRenderBase2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Transform2D& transform, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach = false);
};

class ActorModulateTesselation2D : public ModulatableActor2D
{
	ModulatableActor2D* m_Actor;
	std::vector<std::unique_ptr<Modulator>> m_InternalChildren;

public:
	ActorModulateTesselation2D(ModulatableActor2D* actor);

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline ActorRenderBase2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Modulate& modulate, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Modulate>& modulates, bool sync_on_attach = false);
};

class ActorTesselation2D : public ProteanActor2D
{
	ProteanActor2D* m_Actor;
	std::vector<std::unique_ptr<ProteanLinker2D>> m_InternalChildren;
	
public:
	ActorTesselation2D(ProteanActor2D* actor);
	
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	
	inline ActorRenderBase2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Proteate2D& proteate, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Proteate2D>& proteates, bool sync_on_attach = false);
};
