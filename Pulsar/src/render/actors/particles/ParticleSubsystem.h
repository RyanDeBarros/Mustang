#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "utils/CommonMath.h"
#include "Particle.h"
#include "../../transform/Fickle.inl"

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

// TODO use tesselation for memory efficiency?
template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSubsystem
{
	ParticleSubsystemData<ParticleCount> m_Data;
	real m_PeriodInv;
	ParticleCount m_NumSpawned = 0;
	ParticleCount m_TotalSpawn;
	unsigned int m_WaveNum = 0;
	ParticleSubsystemIndex m_SubsystemIndex;
	FickleSelector2D m_Fickler;
	std::vector<Particle> m_Particles;

public:
	ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index, FickleType fickle_type = FickleType::Protean);
	ParticleSubsystem(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem(ParticleSubsystem<ParticleCount>&&) noexcept;
	ParticleSubsystem<ParticleCount>& operator=(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem<ParticleCount>& operator=(ParticleSubsystem<ParticleCount>&&) noexcept;

	inline void SetWavePeriod(real wave_period) { m_Data.wavePeriod = wave_period; m_PeriodInv = 1.0f / m_Data.wavePeriod; }

	inline FickleSelector2D& Fickler() { return m_Fickler; }

private:
	friend class ParticleEffect<ParticleCount>;
	void OnSpawnFrame(ParticleEffect<ParticleCount>& psys);
	void Spawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed);
	void OnParticlesUpdate(ParticleEffect<ParticleCount>& psys);
	void RemoveUnordered(ParticleCount i);
	
	void BindFickleFunctions();
	void (ParticleSubsystem<ParticleCount>::*f_AttachFickle)(FickleSelector2D&) = nullptr;
	void attach_fickle_protean(FickleSelector2D&);
	void attach_fickle_transformable(FickleSelector2D&);
	void attach_fickle_modulatable(FickleSelector2D&);
	void (ParticleSubsystem<ParticleCount>::*f_FicklerSwapPop)(ParticleCount) = nullptr;
	void fickler_swap_pop_protean(ParticleCount);
	void fickler_swap_pop_transformable(ParticleCount);
	void fickler_swap_pop_modulatable(ParticleCount);
};
