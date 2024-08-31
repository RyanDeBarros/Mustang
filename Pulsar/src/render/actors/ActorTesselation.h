#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <variant>

#include "../ActorRenderBase.h"

class ActorTesselation2D : public FickleActor2D
{
	FickleActor2D* m_Actor;
	std::vector<Fickler2D*> m_InternalPlacements;
	void(*f_RequestDraw)(class CanvasLayer*, ActorTesselation2D*, FickleActor2D*) = nullptr;

public:
	ActorTesselation2D(FickleActor2D* actor, FickleType fickle_type = FickleType::Transformable);
	ActorTesselation2D(const ActorTesselation2D&) = delete;
	ActorTesselation2D(ActorTesselation2D&&) = delete;
	~ActorTesselation2D();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	
	FickleActor2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Transform2D& transform, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach = false);
	void PushBackStatic(const Transform2D& transform, const Modulate& modulate, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<std::pair<Transform2D, Modulate>>& proteates, bool sync_on_attach = false);
	void PushBackStatic(const Modulate& modulate, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Modulate>& modulates, bool sync_on_attach = false);
};
