#pragma once

#include <unordered_map>

#include "render/ActorRenderBase.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "ParticleEffect.h"

class ParticleSubsystemArray: public ParticleEffect
{
	std::vector<DebugBatcher> m_Batchers;
	std::vector<std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>>> m_WaitingForDespawn;

public:
	ParticleSubsystemArray(const std::vector<ParticleSubsystemData>& subsystem_data, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true, bool enabled = true);
	ParticleSubsystemArray(std::vector<ParticleSubsystemData>&& subsystem_data, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true, bool enabled = true);
	ParticleSubsystemArray(const ParticleSubsystemArray&) = delete;
	ParticleSubsystemArray(ParticleSubsystemArray&&) = delete;

	/// Whether the particle system should be drawn. This does not affect whether the system is updated every update frame.
	bool visible;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual void InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape) override;
	virtual void DespawnInvalidParticles() override;
	virtual void AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape) override;
};
