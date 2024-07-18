#pragma once

#include <variant>

#include "Pulsar.h"
#include "render/ActorRenderBase.h"
#include "render/Transform.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "Particle.h"
#include "ParticleWave.h"

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSystem : public ActorRenderBase2D, Transformable2D
{
	DebugBatcher m_Batcher;
	std::vector<Particle> m_Particles;
	std::vector<ParticleWave<ParticleCount>> m_Waves;
	float m_LifetimeStart;
	
public:
	ParticleSystem(const Transform2D& transform = {}, ZIndex z = 0);

	/// Whether the particle system should be drawn. This does not affect whether the system is updated every update frame.
	bool visible;
	/// Whether the particle system is updated every update frame. This does not affect whether the system should be drawn.
	bool enabled;
	
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	void OnUpdate();

	inline void Reset() { m_LifetimeStart = glfwGetTime(); }
};
