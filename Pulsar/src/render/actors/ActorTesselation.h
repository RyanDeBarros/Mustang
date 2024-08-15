#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "../ActorRenderBase.h"

// TODO add modulator implementation

class ActorTesselation2D : public ProteanActor2D
{
	ProteanActor2D* m_Actor;
	std::vector<std::shared_ptr<Transformer2D>> m_InternalChildren;
	
public:
	ActorTesselation2D(ProteanActor2D* actor);
	ActorTesselation2D(const ActorTesselation2D&);
	ActorTesselation2D(ActorTesselation2D&&) noexcept;
	ActorTesselation2D& operator=(const ActorTesselation2D&);
	ActorTesselation2D& operator=(ActorTesselation2D&&) noexcept;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	
	inline ActorRenderBase2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Transform2D& transform, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach = false);
};
