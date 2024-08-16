#include "ParticleSystem.h"

#include "render/CanvasLayer.h"

template class ParticleSystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSystem<ParticleCount>::ParticleSystem(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, ZIndex z, FickleType fickle_type, bool visible, bool enabled)
	: ParticleEffect<ParticleCount>(subsystem_data, fickle_type, enabled), m_Batcher(z), ActorRenderBase2D(z), visible(visible)
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
void ParticleSystem<ParticleCount>::InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	auto iter = m_WaitingForDespawn.find(shape->GetDebugModel());
	if (iter == m_WaitingForDespawn.end())
		m_WaitingForDespawn[shape->GetDebugModel()] = { shape };
	else
		iter->second.insert(shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::DespawnInvalidParticles()
{
	m_Batcher.EraseAll(m_WaitingForDespawn);
	m_WaitingForDespawn.clear();
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	m_Batcher.PushBack(shape);
}
