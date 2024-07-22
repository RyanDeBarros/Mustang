#include "ParticleWave.h"

#include "ParticleSystem.h"

template struct ParticleWaveData<unsigned short>;
template class ParticleWave<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleWave<ParticleCount>::ParticleWave(const ParticleWaveData<ParticleCount>& wave_data)
	: m_Shape(wave_data.prototypeShape), m_SpawnFunc(wave_data.spawnFunc), m_LifespanFunc(wave_data.lifespanFunc), m_CharacteristicsVecGen(wave_data.m_CharacteristicsVecGen), m_TotalSpawn(m_SpawnFunc(1.0f))
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnUpdate(float delta_time, ParticleSystem<ParticleCount>& psys)
{
	unsigned int wave_num = static_cast<unsigned int>(delta_time * m_WavePeriodInv);
	real t = (delta_time - wave_num * m_WavePeriod) * m_WavePeriodInv;
	ParticleCount num_to_spawn = 0;
	if (wave_num > m_WaveNum)
	{
		num_to_spawn = m_SpawnFunc(1.0f);
		for (auto _ = 0; _ < num_to_spawn; _++)
			OnSpawn(t, psys, m_NumSpawned++);
		m_NumSpawned = 0;
		num_to_spawn = m_SpawnFunc(0.0f);
		m_WaveNum = wave_num;
	}
	num_to_spawn += m_SpawnFunc(t);
	for (auto _ = 0; _ < num_to_spawn; _++)
		OnSpawn(t, psys, m_NumSpawned++);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnSpawn(float t, ParticleSystem<ParticleCount>& psys, unsigned int spawn_index)
{
	std::shared_ptr<DebugPolygon> shape(new DebugPolygon(*m_Shape));
	psys.m_Particles.push_back(Particle(shape, LocalTransformer2D(psys.m_Transform, shape), m_LifespanFunc(t), m_CharacteristicsVecGen(t, spawn_index, m_TotalSpawn)));
	psys.m_Batcher.PushBack(shape);
}
