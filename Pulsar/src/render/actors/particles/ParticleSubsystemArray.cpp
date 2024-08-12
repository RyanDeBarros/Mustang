#include "ParticleSubsystemArray.h"

#include "render/CanvasLayer.h"

template class ParticleSubsystemArray<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSubsystemArray<ParticleCount>::ParticleSubsystemArray(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform, const glm::vec4& modulate, ZIndex z, bool visible, bool enabled)
	: ParticleEffect<ParticleCount>(subsystem_data, transform, modulate, enabled), ActorRenderBase2D(z), visible(visible)
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
void ParticleSubsystemArray<ParticleCount>::InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	auto iter = m_WaitingForDespawn[i].find(shape->GetDebugModel());
	if (iter == m_WaitingForDespawn[i].end())
		m_WaitingForDespawn[i][shape->GetDebugModel()] = { shape };
	else
		iter->second.insert(shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::DespawnInvalidParticles()
{
	for (ParticleSubsystemIndex i = 0; i < m_Batchers.size(); i++)
	{
		m_Batchers[i].EraseAll(m_WaitingForDespawn[i]);
		m_WaitingForDespawn[i].clear();
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystemArray<ParticleCount>::AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	m_Batchers[i].PushBack(shape);
}
