#include "ParticleWave.h"

#include "ParticleSystem.h"

template struct ParticleWaveData<unsigned short>;
template class ParticleWave<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleWave<ParticleCount>::ParticleWave(const ParticleWaveData<ParticleCount>& wave_data)
	: m_Shape(wave_data.prototypeShape), m_SpawnFunc(wave_data.spawnFunc), m_LifespanFunc(wave_data.lifespanFunc), m_CharacteristicGen(wave_data.characteristicGen), m_TotalSpawn(m_SpawnFunc(1.0f))
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnUpdate(ParticleSystem<ParticleCount>& psys)
{
	unsigned int wave_num = static_cast<unsigned int>(psys.m_TotalPlayed * m_WavePeriodInv);
	real t = (psys.m_TotalPlayed - wave_num * m_WavePeriod) * m_WavePeriodInv;
	ParticleCount num_to_spawn = 0;
	if (wave_num > m_WaveNum)
	{
		num_to_spawn = m_SpawnFunc(1.0f);
		while (num_to_spawn-- > 0)
			OnSpawn(psys, { t, m_NumSpawned++, m_TotalSpawn });
		m_NumSpawned = 0;
		num_to_spawn = m_SpawnFunc(0.0f);
		m_WaveNum = wave_num;
	}
	num_to_spawn += m_SpawnFunc(t);
	if (num_to_spawn > 0)
	{
		Particles::CHRSeed seed{ t, m_NumSpawned, m_TotalSpawn };
		while (num_to_spawn-- > 0)
		{
			seed.spawnIndex = m_NumSpawned++;
			OnSpawn(psys, seed);
		}
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleWave<ParticleCount>::OnSpawn(ParticleSystem<ParticleCount>& psys, const Particles::CHRSeed& seed)
{
	std::shared_ptr<DebugPolygon> shape(new DebugPolygon(*m_Shape));
	psys.m_Particles.emplace_back(shape, LocalTransformer2D(psys.m_Transform, shape), m_LifespanFunc(seed), m_CharacteristicGen(seed));
	psys.m_Batcher.PushBack(shape);
}
