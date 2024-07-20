#include "ParticleSystem.h"

#include <unordered_set>

#include "ParticleWave.h"

template class ParticleSystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSystem<ParticleCount>::ParticleSystem(const std::vector<ParticleWaveData<ParticleCount>>& wave_data, const Transform2D& transform, ZIndex z, bool visible, bool enabled)
	: m_Batcher(z), ActorRenderBase2D(z), Transformable2D(transform), visible(visible), enabled(enabled)
{
	for (const auto& wave : wave_data)
		m_Waves.emplace_back(wave);
	Reset();
}

// TODO some way of skipping frames for intensive actors like particle systems. i.e., instead of updating every draw frame, an intensive actor can update for say, 120FPS, to limit draw/update overhead. Drawing will still happen obviously, but the update will happen every N frames. Maybe separate the two into separate functions.
template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::RequestDraw(CanvasLayer* canvas_layer)
{
	// TODO perhaps spawning and enabled should be moved to particle wave?
	if (spawning)
		OnWavesUpdate();
	if (enabled)
		OnParticlesUpdate();
	if (visible)
		m_Batcher.RequestDraw(canvas_layer);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::OnWavesUpdate()
{
	real dt = Pulsar::totalDrawTime - m_LifetimeStart;
	if (m_PlayTime > 0.0f && dt > m_PlayTime)
	{
		spawning = false;
		Reset();
		return;
	}
	for (ParticleWave<ParticleCount>& wave : m_Waves)
		wave.OnUpdate(dt, *this);
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::OnParticlesUpdate()
{
	// update particles
	std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>> to_delete;
	for (auto& part : m_Particles)
	{
		part.OnDraw();
		if (part.m_Invalid)
		{
			auto iter = to_delete.find(part.m_Shape->GetDebugModel());
			if (iter == to_delete.end())
				to_delete[part.m_Shape->GetDebugModel()] = { part.m_Shape };
			else
				iter->second.insert(part.m_Shape);
		}
	}
	if (to_delete.size() > 0)
	{
		m_Batcher.EraseAll(to_delete);
		std::erase_if(m_Particles, [](const Particle& part) { return part.m_Invalid; });
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::Pause()
{
	if (enabled || spawning)
	{
		m_PausedAt = Pulsar::totalDrawTime;
		enabled = false;
		spawning = false;
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::Resume()
{
	if (!enabled || !spawning)
	{
		m_LifetimeStart += Pulsar::totalDrawTime - m_PausedAt;
		enabled = true;
		spawning = true;
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSystem<ParticleCount>::PlayFor(real n)
{
	m_PlayTime = n;
	spawning = true;
}
