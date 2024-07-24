#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "Utils.h"
#include "render/actors/shapes/DebugPolygon.h"
#include "render/Transform.h"

// TODO throughout project, pass by value for built-in types instead of const&.
struct Particle;
namespace Particles {

	typedef std::function<void(Particle& p)> CHRFunc;
	typedef std::pair<CHRFunc, unsigned char> CHRBind;
	
	struct CHRSeed
	{
		real waveT;
		unsigned int spawnIndex;
		unsigned int totalSpawn;
	};

	typedef std::function<CHRBind(const CHRSeed&)> CharacteristicGen;

	extern CharacteristicGen CombineSequential(bool combine_data, const std::vector<CharacteristicGen>& characteristics); 
	extern CharacteristicGen CombineInitialOverTime(bool combine_data, const CharacteristicGen& initial, const CharacteristicGen& over_time);
	extern CharacteristicGen CombineIntervals(bool combine_data, const CharacteristicGen& first, const CharacteristicGen& second, float divider, bool or_equal = false);
}

struct Particle
{
	LocalTransformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	Particles::CHRFunc m_Characteristic;
	float* m_Data = nullptr;
	unsigned char m_DataSize;

private:
	template<std::unsigned_integral ParticleCount>
	friend class ParticleSystem;
	real m_LifespanInv;
	real m_T = 0.0f;
	real m_DT = 0.0f;
	bool m_Invalid = false;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan, const Particles::CHRBind& characteristic);
	Particle(const Particle&) = delete;
	Particle(Particle&&) noexcept;
	~Particle();
	Particle& operator=(const Particle&);

	inline float& operator[](unsigned char i) { return m_Data[i]; }
	
	inline real t() const { return m_T; }
	inline real dt() const { return m_DT; }

private:
	void OnDraw(real delta_time);

};
