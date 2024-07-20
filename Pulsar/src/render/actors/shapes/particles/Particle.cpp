#include "Particle.h"

#include "Pulsar.h"
#include "Utils.h"

Particle::Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan,
		const std::vector<Particles::Characteristic>& characteristics)
	: m_Shape(shape), m_Transformer(transformer), m_Characteristics(characteristics), m_LifespanInv(1.0f / lifespan)
{
	for (const auto& func : characteristics)
		func(*this);
}

Particle& Particle::operator=(const Particle& other)
{
	m_Shape = other.m_Shape;
	m_Transformer = other.m_Transformer;
	m_Characteristics = other.m_Characteristics;
	m_LifespanInv = other.m_LifespanInv;
	m_Invalid = other.m_Invalid;
	m_T = other.m_T;
	m_DT = other.m_DT;
	return *this;
}

void Particle::OnDraw()
{
	m_DT = Pulsar::deltaDrawTime * m_LifespanInv;
	if ((m_T += m_DT) > 1.0f)
		m_Invalid = true;
	else
	{
		for (const auto& func : m_Characteristics)
			func(*this);
	}
}
