#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "utils/CommonMath.h"
#include "Particle.h"
#include "../../transform/MultiTransformer.h"

typedef unsigned char ParticleSubsystemIndex;

template<std::unsigned_integral ParticleCount = unsigned short>
struct ParticleSubsystemData
{
	real wavePeriod;
	std::shared_ptr<DebugPolygon> prototypeShape;
	CumulativeFunc<ParticleCount> spawnFunc;
	std::function<real(const Particles::CHRSeed&)> lifespanFunc;
	Particles::CharacteristicGen characteristicGen;
};

template<std::unsigned_integral ParticleCount>
class ParticleEffect;

// TODO just use ParticleSubsystemData as a data member of ParticleSubsystem, to avoid all this repeating.
template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSubsystem
{
	CumulativeFunc<ParticleCount> m_SpawnFunc;
	std::function<real(const Particles::CHRSeed&)> m_LifespanFunc;
	Particles::CharacteristicGen m_CharacteristicGen;
	real m_Period;
	real m_PeriodInv;
	std::shared_ptr<DebugPolygon> m_Shape;
	ParticleCount m_NumSpawned = 0;
	unsigned int m_TotalSpawn;
	unsigned int m_WaveNum = 0;
	ParticleSubsystemIndex m_SubsystemIndex;
	std::shared_ptr<TransformableProxy2D> m_Transform;
	std::shared_ptr<MultiTransformer2D> m_Transformer;
	std::vector<Particle> m_Particles;

public:
	ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index);
	ParticleSubsystem(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem(ParticleSubsystem<ParticleCount>&&) noexcept;
	ParticleSubsystem<ParticleCount>& operator=(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem<ParticleCount>& operator=(ParticleSubsystem<ParticleCount>&&) noexcept;

	inline void SetWavePeriod(real wave_period) { m_Period = wave_period; m_PeriodInv = 1.0f / m_Period; }

private:
	friend class ParticleEffect<ParticleCount>;
	void OnSpawnFrame(ParticleEffect<ParticleCount>& psys);
	void Spawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed);
	void OnParticlesUpdate(ParticleEffect<ParticleCount>& psys);
	void RemoveUnordered(ParticleCount i);
};
