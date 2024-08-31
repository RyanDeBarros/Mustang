#include "ParticleSystem.h"

#include "render/CanvasLayer.h"

ParticleSystem::ParticleSystem(const std::vector<ParticleSubsystemData>& subsystem_data, ZIndex z, FickleType fickle_type, bool visible, bool enabled)
	: ParticleEffect(subsystem_data, z, fickle_type, enabled), m_Batcher(z), visible(visible)
{
}

ParticleSystem::ParticleSystem(std::vector<ParticleSubsystemData>&& subsystem_data, ZIndex z, FickleType fickle_type, bool visible, bool enabled)
	: ParticleEffect(std::move(subsystem_data), z, fickle_type, enabled), m_Batcher(z), visible(visible)
{
}

void ParticleSystem::RequestDraw(CanvasLayer* canvas_layer)
{
	ParticleEffect::OnUpdate();
	if (visible)
		m_Batcher.RequestDraw(canvas_layer);
}

void ParticleSystem::InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	auto iter = m_WaitingForDespawn.find(shape->GetDebugModel());
	if (iter == m_WaitingForDespawn.end())
		m_WaitingForDespawn[shape->GetDebugModel()] = { shape };
	else
		iter->second.insert(shape);
}

void ParticleSystem::DespawnInvalidParticles()
{
	m_Batcher.EraseAll(m_WaitingForDespawn);
	m_WaitingForDespawn.clear();
}

void ParticleSystem::AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	m_Batcher.PushBack(shape);
}
