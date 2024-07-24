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

	extern CharacteristicGen CombineSequential(const std::vector<CharacteristicGen>& characteristics); 
	extern CharacteristicGen CombineInitialOverTime(const CharacteristicGen& initial, const CharacteristicGen& over_time);
}

struct CHRData
{
	float* s = nullptr;
	unsigned char n;
	CHRData(unsigned char n) : n(n) { if (n > 0) { s = new float[n]; } }
	CHRData(const CHRData& data) : n(data.n)
	{
		if (n > 0)
		{
			s = new float[n];
			memcpy_s(s, n, data.s, n);
		}
	}
	CHRData(CHRData&& data) noexcept : n(data.n), s(data.s) { data.s = nullptr; }
	~CHRData() { if (s) delete[] s; }
	CHRData& operator=(const CHRData& other)
	{
		n = other.n;
		if (n > 0)
		{
			s = new float[n];
			memcpy_s(s, n, other.s, n);
		}
		else
		{
			s = nullptr;
		}
		return *this;
	}
};

struct Particle
{
	LocalTransformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	Particles::CHRFunc m_Characteristic;
	CHRData m_Data;

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
	Particle& operator=(const Particle&);
	
	inline real t() const { return m_T; }
	inline real dt() const { return m_DT; }
	inline CHRData& data() { return m_Data; }

private:
	void OnDraw();

};
