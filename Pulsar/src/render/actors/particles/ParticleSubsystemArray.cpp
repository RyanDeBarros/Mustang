#include "ParticleSubsystemArray.h"

#include "render/CanvasLayer.h"

ParticleSubsystemArray::ParticleSubsystemArray(const std::vector<ParticleSubsystemData>& subsystem_data, ZIndex z, FickleType fickle_type, bool visible, bool enabled)
	: ParticleEffect(subsystem_data, z, fickle_type, enabled), visible(visible)
{
	for (const auto& _ : subsystem_data)
	{
		m_Batchers.push_back(DebugBatcher(z));
		m_WaitingForDespawn.push_back({});
	}
}

ParticleSubsystemArray::ParticleSubsystemArray(std::vector<ParticleSubsystemData>&& subsystem_data, ZIndex z, FickleType fickle_type, bool visible, bool enabled)
	: ParticleEffect(std::move(subsystem_data), z, fickle_type, enabled), visible(visible)
{
	for (size_t _ = 0; _ < m_Subsystems.size(); _++)
	{
		m_Batchers.push_back(DebugBatcher(z));
		m_WaitingForDespawn.push_back({});
	}
}

void ParticleSubsystemArray::RequestDraw(CanvasLayer* canvas_layer)
{
	ParticleEffect::OnUpdate();
	if (visible)
	{
		for (auto& batcher : m_Batchers)
			batcher.RequestDraw(canvas_layer);
	}
}

void ParticleSubsystemArray::InvalidateParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	auto iter = m_WaitingForDespawn[i].find(shape->GetDebugModel());
	if (iter == m_WaitingForDespawn[i].end())
		m_WaitingForDespawn[i][shape->GetDebugModel()] = { shape };
	else
		iter->second.insert(shape);
}

void ParticleSubsystemArray::DespawnInvalidParticles()
{
	for (ParticleSubsystemIndex i = 0; i < m_Batchers.size(); i++)
	{
		m_Batchers[i].EraseAll(m_WaitingForDespawn[i]);
		m_WaitingForDespawn[i].clear();
	}
}

void ParticleSubsystemArray::AddParticleShape(ParticleSubsystemIndex i, const std::shared_ptr<DebugPolygon>& shape)
{
	m_Batchers[i].PushBack(shape);
}
