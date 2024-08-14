#include "ParticleSubsystem.h"

#include "ParticleSystem.h"
#include "Logger.h"
#include "utils/Data.h"

template struct ParticleSubsystemData<unsigned short>;
template class ParticleSubsystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index)
	: m_Data(wave_data), m_TotalSpawn(m_Data.spawnFunc(1.0f)), m_SubsystemIndex(subsystem_index), m_Transformer(), m_Modulator()
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(const ParticleSubsystem<ParticleCount>& other)
	: m_Data(other.m_Data), m_PeriodInv(other.m_PeriodInv), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum), m_SubsystemIndex(other.m_SubsystemIndex), m_Transformer(other.m_Transformer), m_Modulator(other.m_Modulator)
{
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(ParticleSubsystem<ParticleCount>&& other) noexcept
	: m_Data(std::move(other.m_Data)), m_PeriodInv(other.m_PeriodInv), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum), m_SubsystemIndex(other.m_SubsystemIndex), m_Transformer(std::move(other.m_Transformer)), m_Modulator(std::move(other.m_Modulator))
{
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>& ParticleSubsystem<ParticleCount>::operator=(const ParticleSubsystem<ParticleCount>& other)
{
	m_Data = other.m_Data;
	m_PeriodInv = other.m_PeriodInv;
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Transformer = other.m_Transformer;
	m_Modulator = other.m_Modulator;
	return *this;
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>& ParticleSubsystem<ParticleCount>::operator=(ParticleSubsystem<ParticleCount>&& other) noexcept
{
	m_Data = std::move(other.m_Data);
	m_PeriodInv = other.m_PeriodInv;
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Transformer = std::move(other.m_Transformer);
	m_Modulator = std::move(other.m_Modulator);
	return *this;
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::OnSpawnFrame(ParticleEffect<ParticleCount>& psys)
{
	unsigned int wave_num = static_cast<unsigned int>(psys.m_TotalPlayed * m_PeriodInv);
	real t = (psys.m_TotalPlayed - wave_num * m_Data.wavePeriod) * m_PeriodInv;
	ParticleCount num_to_spawn = 0;
	if (wave_num > m_WaveNum)
	{
		num_to_spawn = m_Data.spawnFunc(1.0f);
		while (num_to_spawn-- > 0)
			Spawn(psys, { t, m_NumSpawned++, m_TotalSpawn });
		m_NumSpawned = 0;
		num_to_spawn = m_Data.spawnFunc(0.0f);
		m_WaveNum = wave_num;
	}
	num_to_spawn += m_Data.spawnFunc(t);
	if (num_to_spawn > 0)
	{
		Particles::CHRSeed seed{ t, m_NumSpawned, m_TotalSpawn };
		while (num_to_spawn-- > 0)
		{
			seed.spawnIndex = m_NumSpawned++;
			Spawn(psys, seed);
		}
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::Spawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed)
{
	std::shared_ptr<DebugPolygon> shape(std::make_shared<DebugPolygon>(*m_Data.prototypeShape));
	m_Transformer.Attach(shape->Transformer());
	m_Modulator.Attach(shape->Modulator());
	m_Particles.push_back(Particle(shape, m_Data.lifespanFunc(seed), m_Data.characteristicGen(seed)));
	psys.AddParticleShape(m_SubsystemIndex, shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::OnParticlesUpdate(ParticleEffect<ParticleCount>& psys)
{
	for (ParticleCount i = 0; i < m_Particles.size(); i++)
	{
		auto& p = m_Particles[i];
		p.OnDraw(psys.m_DeltaTime);
		if (p.m_Invalid)
		{
			psys.InvalidateParticleShape(m_SubsystemIndex, p.m_Shape);
			RemoveUnordered(i);
			i--;
		}
	}
	psys.DespawnInvalidParticles();
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::RemoveUnordered(ParticleCount i)
{
	if (m_Particles.size() > 1)
		std::swap(m_Particles[i], m_Particles.back());
	m_Particles.pop_back();
	swap_pop(m_Transformer.children, i);
	swap_pop(m_Modulator.children, i);
}
