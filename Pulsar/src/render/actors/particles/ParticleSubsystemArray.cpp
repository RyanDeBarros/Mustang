#include "ParticleSubsystemArray.h"

#include "render/CanvasLayer.h"

template class ParticleSubsystemArray<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSubsystemArray<ParticleCount>::ParticleSubsystemArray(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform, ZIndex z, bool visible, bool enabled)
	: ParticleEffect<ParticleCount>(subsystem_data, transform, enabled), ActorRenderBase2D(z), visible(visible)
{
	for (const auto& _ : subsystem_data)
	{
		m_Batchers.push_back(DebugBatcher(z));
		m_WaitingForDespawn.push_back({});
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::RequestDraw(CanvasLayer* canvas_layer)
{
	ParticleEffect<ParticleCount>::OnUpdate();
	if (visible)
	{
		for (auto& batcher : m_Batchers)
			batcher.RequestDraw(canvas_layer);
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::InvalidateParticle(const Particle& part, ParticleSubsystemIndex i)
{
	auto iter = m_WaitingForDespawn[i].find(part.m_Shape->GetDebugModel());
	if (iter == m_WaitingForDespawn[i].end())
		m_WaitingForDespawn[i][part.m_Shape->GetDebugModel()] = { part.m_Shape };
	else
		iter->second.insert(part.m_Shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::DespawnInvalidParticles()
{
	for (ParticleSubsystemIndex i = 0; i < m_Batchers.size(); i++)
	{
		m_Batchers[i].EraseAll(m_WaitingForDespawn[i]);
		m_WaitingForDespawn[i].clear();
	}
	ParticleEffect<ParticleCount>::DespawnInvalidParticles();
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::AddParticle(ParticleSubsystemIndex i, const Particle& part)
{
	m_Batchers[i].PushBack(part.m_Shape);
	ParticleEffect<ParticleCount>::AddParticle(i, part);
}
