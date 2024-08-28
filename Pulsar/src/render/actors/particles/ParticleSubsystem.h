#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Pulsar.h"
#include "utils/CommonMath.h"
#include "Particle.h"
#include "../../transform/Fickle.inl"

typedef unsigned char ParticleSubsystemIndex;
typedef unsigned int ParticleCount;

struct ParticleSubsystemData
{
	real wavePeriod;
	std::shared_ptr<DebugPolygon> prototypeShape;
	CumulativeFunc<ParticleCount> spawnFunc;
	FunctorPtr<real, const Particles::CHRSeed&> lifespanFunc;
	Particles::CharacteristicGen characteristicGen;
};

class ParticleEffect;

// TODO use tesselation for memory efficiency?
class ParticleSubsystem
{
	ParticleSubsystemData m_Data;
	real m_PeriodInv;
	ParticleCount m_NumSpawned = 0;
	ParticleCount m_TotalSpawn;
	unsigned short m_WaveNum = 0;
	ParticleSubsystemIndex m_SubsystemIndex;
	FickleSelector2D m_Fickler;
	std::vector<Particle> m_Particles;

public:
	ParticleSubsystem(const ParticleSubsystemData& wave_data, ParticleSubsystemIndex subsystem_index, FickleType fickle_type = FickleType::Protean);
	ParticleSubsystem(const ParticleSubsystem&);
	ParticleSubsystem(ParticleSubsystem&&) noexcept;
	ParticleSubsystem& operator=(const ParticleSubsystem&);
	ParticleSubsystem& operator=(ParticleSubsystem&&) noexcept;

	inline void SetWavePeriod(real wave_period) { m_Data.wavePeriod = wave_period; m_PeriodInv = 1.0f / m_Data.wavePeriod; }

	inline FickleSelector2D& Fickler() { return m_Fickler; }

private:
	friend class ParticleEffect;
	void OnSpawnFrame(ParticleEffect& psys);
	void Spawn(ParticleEffect& psys, const Particles::CHRSeed& seed);
	void OnParticlesUpdate(ParticleEffect& psys);
	void RemoveUnordered(ParticleCount i);
	
	void BindFickleFunctions();
	void (ParticleSubsystem::*f_AttachFickle)(FickleSelector2D&) = nullptr;
	void attach_fickle_protean(FickleSelector2D&);
	void attach_fickle_transformable(FickleSelector2D&);
	void attach_fickle_modulatable(FickleSelector2D&);
	void (ParticleSubsystem::*f_FicklerSwapPop)(ParticleCount) = nullptr;
	void fickler_swap_pop_protean(ParticleCount);
	void fickler_swap_pop_transformable(ParticleCount);
	void fickler_swap_pop_modulatable(ParticleCount);
};
