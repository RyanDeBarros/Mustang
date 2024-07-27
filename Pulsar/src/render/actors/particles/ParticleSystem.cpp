#include "ParticleSystem.h"

#include "render/CanvasLayer.h"

template class ParticleSystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSystem<ParticleCount>::ParticleSystem(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform, ZIndex z, bool visible, bool enabled)
	: ParticleEffect<ParticleCount>(subsystem_data, transform, enabled), m_Batcher(z), ActorRenderBase2D(z), visible(visible)
{
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::RequestDraw(CanvasLayer* canvas_layer)
{
	ParticleEffect<ParticleCount>::OnUpdate();
	if (visible)
		m_Batcher.RequestDraw(canvas_layer);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::InvalidateParticle(const Particle& part, ParticleSubsystemIndex i)
{
	auto iter = m_WaitingForDespawn.find(part.m_Shape->GetDebugModel());
	if (iter == m_WaitingForDespawn.end())
		m_WaitingForDespawn[part.m_Shape->GetDebugModel()] = { part.m_Shape };
	else
		iter->second.insert(part.m_Shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::DespawnInvalidParticles()
{
	m_Batcher.EraseAll(m_WaitingForDespawn);
	m_WaitingForDespawn.clear();
	ParticleEffect<ParticleCount>::DespawnInvalidParticles();
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::AddParticle(ParticleSubsystemIndex i, const Particle& part)
{
	m_Batcher.PushBack(std::move(part.m_Shape));
	ParticleEffect<ParticleCount>::AddParticle(i, part);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::AddParticle(ParticleSubsystemIndex i, Particle&& part)
{
	m_Batcher.PushBack(std::move(part.m_Shape));
	ParticleEffect<ParticleCount>::AddParticle(i, std::move(part));
}
