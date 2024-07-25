#include "ParticleSubsystem.h"

#include "ParticleSystem.h"

template struct ParticleSubsystemData<unsigned short>;
template class ParticleSubsystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index)
	: m_Shape(wave_data.prototypeShape), m_SpawnFunc(wave_data.spawnFunc), m_LifespanFunc(wave_data.lifespanFunc), m_CharacteristicGen(wave_data.characteristicGen), m_TotalSpawn(m_SpawnFunc(1.0f)), m_SubsystemIndex(subsystem_index)
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::OnUpdate(ParticleEffect<ParticleCount>& psys)
{
	unsigned int wave_num = static_cast<unsigned int>(psys.m_TotalPlayed * m_PeriodInv);
	real t = (psys.m_TotalPlayed - wave_num * m_Period) * m_PeriodInv;
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
void ParticleSubsystem<ParticleCount>::OnSpawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed)
{
	std::shared_ptr<DebugPolygon> shape(new DebugPolygon(*m_Shape));
	psys.AddParticle(m_SubsystemIndex, Particle(shape, LocalTransformer2D(psys.m_Transform, shape), m_LifespanFunc(seed), m_CharacteristicGen(seed)));
}
