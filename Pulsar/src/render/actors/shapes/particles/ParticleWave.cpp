#include "ParticleWave.h"

#include <cmath>

template<std::unsigned_integral ParticleCount>
ParticleWave<ParticleCount>::ParticleWave(float wave_period, const DebugPolygon& prototype_shape, const CumulativeFunc<ParticleCount>& spawn_func, const Particles::FloatFunc& lifespan_func, const Particles::ProfileFuncVectorFunc& profile_funcs, const Particles::ProfileFuncVectorFunc& init_funcs)
	: m_WavePeriod(wave_period), m_Shape(prototype_shape), m_SpawnFunc(spawn_func), m_LifespanFunc(lifespan_func), m_ProfileFuncs(profile_funcs), m_InitFuncs(init_funcs)
{
	SetWavePeriod(wave_period);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnUpdate(float deltaTime, Transform2D const* const parent_transform, std::vector<Particle>& particles)
{
	auto t = std::fmod(deltaTime, m_WavePeriod) * m_WavePeriodInv;
	auto num_to_spawn = m_SpawnFunc(t);
	for (auto _ = 0; _ < num_to_spawn; _++)
		OnSpawn(t, parent_transform, particles);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnSpawn(float t, Transform2D const* const parent_transform, std::vector<Particle>& particles)
{
	std::shared_ptr<DebugPolygon> shape(new DebugPolygon(m_Shape));
	float seed = rng();
	particles.push_back(Particle(shape, LocalTransformer2D(parent_transform, shape.get()), m_LifespanFunc(t, seed), m_ProfileFuncs(t, seed), m_InitFuncs(t, seed)));
}
