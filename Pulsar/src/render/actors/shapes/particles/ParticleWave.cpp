#include "ParticleWave.h"

#include <cmath>

#include "ParticleSystem.h"

template struct ParticleWaveData<unsigned short>;
template class ParticleWave<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleWave<ParticleCount>::ParticleWave(const ParticleWaveData<ParticleCount>& wave_data)
	: m_Shape(wave_data.prototypeShape), m_SpawnFunc(wave_data.spawnFunc), m_LifespanFunc(wave_data.lifespanFunc), m_ProfileFuncs(wave_data.profileFuncs), m_InitFuncs(wave_data.initFuncs)
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnUpdate(float delta_time, ParticleSystem<ParticleCount>& psys)
{
	auto t = std::fmod(delta_time, m_WavePeriod) * m_WavePeriodInv;
	auto num_to_spawn = m_SpawnFunc(t);
	for (auto _ = 0; _ < num_to_spawn; _++)
		OnSpawn(t, psys);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnSpawn(float t, ParticleSystem<ParticleCount>& psys)
{
	std::shared_ptr<DebugPolygon> shape(new DebugPolygon(*m_Shape.get()));
	float seed = rng();
	psys.m_Particles.push_back(Particle(shape, LocalTransformer2D(&psys.m_Transform, shape.get()), m_LifespanFunc(t, seed), m_ProfileFuncs(t, seed), m_InitFuncs(t, seed)));
	psys.m_Batcher.PushBack(shape);
}
