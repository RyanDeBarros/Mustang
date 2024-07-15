#pragma once

#include "render/ActorRenderBase.h"
#include "render/Transform.h"
#include "Particle.h"

class ParticleSystem : public ActorRenderBase2D, Transformable2D
{
	ZIndex m_Z;
	Transform2D m_Transform;
	std::vector<std::pair<Particle, LocalTransformer2D>> m_Particles;

public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	inline virtual ZIndex GetZIndex() const override { return m_Z; }
	inline virtual void SetZIndex(const ZIndex& z) override { m_Z = z; }
};
