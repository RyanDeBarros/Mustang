#pragma once

#include <unordered_map>

#include "render/ActorRenderBase.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "ParticleEffect.h"

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSystem : public ActorRenderBase2D, public ParticleEffect<ParticleCount>
{
	DebugBatcher m_Batcher;
	std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>> m_WaitingForDespawn;

public:
	ParticleSystem(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform = {}, const glm::vec4& modulate = Colors::WHITE, ZIndex z = 0, bool visible = true, bool enabled = true);
	ParticleSystem(const ParticleSystem<ParticleCount>&) = delete;
	ParticleSystem(ParticleSystem<ParticleCount>&&) = delete;

	/// Whether the particle system should be drawn. This does not affect whether the system is updated every update frame.
	bool visible;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual void InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape) override;
	virtual void DespawnInvalidParticles() override;
	virtual void AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape) override;
};
