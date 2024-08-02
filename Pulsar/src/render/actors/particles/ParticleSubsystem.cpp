#include "ParticleSubsystem.h"

#include "ParticleSystem.h"
#include "Logger.h"

template struct ParticleSubsystemData<unsigned short>;
template class ParticleSubsystem<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index)
	: m_Shape(wave_data.prototypeShape), m_SpawnFunc(wave_data.spawnFunc), m_LifespanFunc(wave_data.lifespanFunc), m_CharacteristicGen(wave_data.characteristicGen), m_TotalSpawn(m_SpawnFunc(1.0f)), m_SubsystemIndex(subsystem_index), m_Transform(std::make_shared<TransformableProxy2D>()), m_Transformer(std::make_shared<MultiTransformer2D>(m_Transform))
{
	SetWavePeriod(wave_data.wavePeriod);
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(const ParticleSubsystem<ParticleCount>& other)
	: m_SpawnFunc(other.m_SpawnFunc), m_LifespanFunc(other.m_LifespanFunc), m_CharacteristicGen(other.m_CharacteristicGen), m_Period(other.m_Period), m_PeriodInv(other.m_PeriodInv), m_Shape(other.m_Shape), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum), m_SubsystemIndex(other.m_SubsystemIndex), m_Transform(std::make_shared<TransformableProxy2D>(other.m_Transform->GetTransform())), m_Transformer(std::make_shared<MultiTransformer2D>(m_Transform))
{
}

// TODO throughout project, use std::move and r-value function overloads for std::function parameters, unless they are bound to data.

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>::ParticleSubsystem(ParticleSubsystem<ParticleCount>&& other) noexcept
	: m_SpawnFunc(std::move(other.m_SpawnFunc)), m_LifespanFunc(std::move(other.m_LifespanFunc)), m_CharacteristicGen(std::move(other.m_CharacteristicGen)), m_Period(other.m_Period), m_PeriodInv(other.m_PeriodInv), m_Shape(std::move(other.m_Shape)), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum), m_SubsystemIndex(other.m_SubsystemIndex), m_Transform(std::move(other.m_Transform)), m_Transformer(std::make_shared<MultiTransformer2D>(m_Transform))
{
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>& ParticleSubsystem<ParticleCount>::operator=(const ParticleSubsystem<ParticleCount>& other)
{
	m_SpawnFunc = other.m_SpawnFunc;
	m_LifespanFunc = other.m_LifespanFunc;
	m_CharacteristicGen = other.m_CharacteristicGen;
	m_Period = other.m_Period;
	m_PeriodInv = other.m_PeriodInv;
	m_Shape = other.m_Shape;
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Transform = other.m_Transform;
	m_Transformer = other.m_Transformer;
	return *this;
}

template<std::unsigned_integral ParticleCount>
ParticleSubsystem<ParticleCount>& ParticleSubsystem<ParticleCount>::operator=(ParticleSubsystem<ParticleCount>&& other) noexcept
{
	m_SpawnFunc = std::move(other.m_SpawnFunc);
	m_LifespanFunc = std::move(other.m_LifespanFunc);
	m_CharacteristicGen = std::move(other.m_CharacteristicGen);
	m_Period = other.m_Period;
	m_PeriodInv = other.m_PeriodInv;
	m_Shape = std::move(other.m_Shape);
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Transform = std::move(other.m_Transform);
	m_Transformer = std::move(other.m_Transformer);
	return *this;
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::OnSpawnFrame(ParticleEffect<ParticleCount>& psys)
{
	unsigned int wave_num = static_cast<unsigned int>(psys.m_TotalPlayed * m_PeriodInv);
	real t = (psys.m_TotalPlayed - wave_num * m_Period) * m_PeriodInv;
	ParticleCount num_to_spawn = 0;
	if (wave_num > m_WaveNum)
	{
		num_to_spawn = m_SpawnFunc(1.0f);
		while (num_to_spawn-- > 0)
			Spawn(psys, { t, m_NumSpawned++, m_TotalSpawn });
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
			Spawn(psys, seed);
		}
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::Spawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed)
{
	std::shared_ptr<DebugPolygon> shape(std::make_shared<DebugPolygon>(*m_Shape));
	m_Transformer->PushBackGlobal(shape->TransformWeak());
	m_Particles.push_back(Particle(shape, m_LifespanFunc(seed), m_CharacteristicGen(seed), m_Transformer.get()));
	psys.AddParticleShape(m_SubsystemIndex, shape);
}

template<std::unsigned_integral ParticleCount>
void ParticleSubsystem<ParticleCount>::OnParticlesUpdate(ParticleEffect<ParticleCount>& psys)
{
	for (ParticleCount i = 0; i < m_Particles.size(); i++)
	{
		auto& p = m_Particles[i];
		p.OnDraw(psys.m_DeltaTime, i);
		if (p.m_Invalid)
		{
			psys.InvalidateParticleShape(m_SubsystemIndex, p.m_Shape);
			// TODO does not preserve order. Use boolean to decide whether subsystem should be ordered or not (unordered is faster, but may look weird for slow/low-count systems). Use marking and std::erase_if for ordered subsystems.
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
	m_Transformer->SwapPop(i);
}
