#pragma once

#include "render/ActorRenderBase.h"
#include "render/Transform.h"
#include "Particle.h"

class ParticleSystem : public ActorRenderBase2D, Transformable2D
{
	std::vector<std::pair<Particle, LocalTransformer2D>> m_Particles;

public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
};
