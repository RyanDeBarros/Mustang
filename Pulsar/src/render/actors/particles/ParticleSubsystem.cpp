#include "ParticleSubsystem.h"

#include "Macros.h"
#include "ParticleSystem.h"
#include "Logger.inl"
#include "utils/Data.inl"

// TODO r-value wave_data overload?
ParticleSubsystem::ParticleSubsystem(const ParticleSubsystemData& wave_data, ParticleSubsystemIndex subsystem_index, FickleType fickle_type)
	: m_Data(wave_data), m_TotalSpawn(m_Data.spawnFunc(1.0f)), m_SubsystemIndex(subsystem_index), m_Fickler(fickle_type)
{
	PULSAR_ASSERT(wave_data.prototypeShape->Fickler().Type() == fickle_type);
	SetWavePeriod(wave_data.wavePeriod);
	BindFickleFunctions();
}

ParticleSubsystem::ParticleSubsystem(const ParticleSubsystem& other)
	: m_Data(other.m_Data), m_PeriodInv(other.m_PeriodInv), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum),
	m_SubsystemIndex(other.m_SubsystemIndex), m_Fickler(other.m_Fickler), f_AttachFickle(other.f_AttachFickle), f_FicklerSwapPop(other.f_FicklerSwapPop)
{
}

ParticleSubsystem::ParticleSubsystem(ParticleSubsystem&& other) noexcept
	: m_Data(std::move(other.m_Data)), m_PeriodInv(other.m_PeriodInv), m_NumSpawned(other.m_NumSpawned), m_TotalSpawn(other.m_TotalSpawn), m_WaveNum(other.m_WaveNum),
	m_SubsystemIndex(other.m_SubsystemIndex), m_Fickler(std::move(other.m_Fickler)), f_AttachFickle(other.f_AttachFickle), f_FicklerSwapPop(other.f_FicklerSwapPop)
{
}

ParticleSubsystem& ParticleSubsystem::operator=(const ParticleSubsystem& other)
{
	if (this == &other)
		return *this;
	m_Data = other.m_Data;
	m_PeriodInv = other.m_PeriodInv;
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Fickler = other.m_Fickler;
	f_AttachFickle = other.f_AttachFickle;
	f_FicklerSwapPop = other.f_FicklerSwapPop;
	return *this;
}

ParticleSubsystem& ParticleSubsystem::operator=(ParticleSubsystem&& other) noexcept
{
	if (this == &other)
		return *this;
	m_Data = std::move(other.m_Data);
	m_PeriodInv = other.m_PeriodInv;
	m_NumSpawned = other.m_NumSpawned;
	m_TotalSpawn = other.m_TotalSpawn;
	m_WaveNum = other.m_WaveNum;
	m_SubsystemIndex = other.m_SubsystemIndex;
	m_Fickler = std::move(other.m_Fickler);
	f_AttachFickle = other.f_AttachFickle;
	f_FicklerSwapPop = other.f_FicklerSwapPop;
	return *this;
}

void ParticleSubsystem::OnSpawnFrame(ParticleEffect& psys)
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

void ParticleSubsystem::Spawn(ParticleEffect& psys, const Particles::CHRSeed& seed)
{
	std::shared_ptr<DebugPolygon> shape(std::make_shared<DebugPolygon>(*m_Data.prototypeShape));
	(this->*f_AttachFickle)(shape->Fickler());
	m_Particles.push_back(Particle(shape, m_Data.lifespanFunc(seed), m_Data.characteristicGen(seed)));
	psys.AddParticleShape(m_SubsystemIndex, shape);
}

void ParticleSubsystem::OnParticlesUpdate(ParticleEffect& psys)
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

void ParticleSubsystem::RemoveUnordered(ParticleCount i)
{
	if (m_Particles.size() > 1)
		std::swap(m_Particles[i], m_Particles.back());
	m_Particles.pop_back();
	(this->*f_FicklerSwapPop)(i);
}

void ParticleSubsystem::BindFickleFunctions()
{
	switch (m_Fickler.Type())
	{
	case FickleType::Protean:
		f_AttachFickle = &ParticleSubsystem::attach_fickle_protean;
		f_FicklerSwapPop = &ParticleSubsystem::fickler_swap_pop_protean;
		break;
	case FickleType::Transformable:
		f_AttachFickle = &ParticleSubsystem::attach_fickle_transformable;
		f_FicklerSwapPop = &ParticleSubsystem::fickler_swap_pop_transformable;
		break;
	case FickleType::Modulatable:
		f_AttachFickle = &ParticleSubsystem::attach_fickle_modulatable;
		f_FicklerSwapPop = &ParticleSubsystem::fickler_swap_pop_modulatable;
		break;
	}
}

void ParticleSubsystem::attach_fickle_protean(Fickler2D& fickler)
{
	m_Fickler.AttachUnsafe(fickler);
}

void ParticleSubsystem::attach_fickle_transformable(Fickler2D& fickler)
{
	m_Fickler.AttachTransformerUnsafe(fickler);
}

void ParticleSubsystem::attach_fickle_modulatable(Fickler2D& fickler)
{
	m_Fickler.AttachModulatorUnsafe(fickler);
}

void ParticleSubsystem::fickler_swap_pop_protean(ParticleCount i)
{
	swap_pop(m_Fickler.Transformer()->children, i);
	swap_pop(m_Fickler.Modulator()->children, i);
}

void ParticleSubsystem::fickler_swap_pop_transformable(ParticleCount i)
{
	swap_pop(m_Fickler.Transformer()->children, i);
}

void ParticleSubsystem::fickler_swap_pop_modulatable(ParticleCount i)
{
	swap_pop(m_Fickler.Modulator()->children, i);
}
