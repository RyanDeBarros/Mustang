#include "Particle.h"

#include "Pulsar.h"
#include "utils/Strings.h"

Particle::Particle(const std::shared_ptr<DebugPolygon>& shape, const float& lifespan, Particles::CHRBind&& characteristic)
	: m_Shape(shape), m_Characteristic(std::move(characteristic.first)), m_LifespanInv(1.0f / lifespan), m_DataSize(characteristic.second)
{
	if (m_DataSize > 0)
		m_Data = new float[m_DataSize];
	m_Characteristic(*this);
}

//Particle::Particle(const Particle& other)
//	: m_Shape(std::make_shared<DebugPolygon>(*other.m_Shape)), m_Characteristic(other.m_Characteristic), m_LifespanInv(other.m_LifespanInv),
//	m_DataSize(other.m_DataSize), m_Invalid(other.m_Invalid), m_T(other.m_T), m_DT(other.m_DT)
//{
//	if (m_DataSize > 0)
//	{
//		m_Data = new float[m_DataSize];
//		memcpy_s(m_Data, m_DataSize * sizeof(float), other.m_Data, m_DataSize * sizeof(float));
//	}
//	(*m_Characteristic)(*this);
//}

Particle::Particle(Particle&& other) noexcept
	: m_Shape(std::move(other.m_Shape)), m_Characteristic(std::move(other.m_Characteristic)), m_LifespanInv(other.m_LifespanInv),
	m_Data(other.m_Data), m_DataSize(other.m_DataSize), m_Invalid(other.m_Invalid), m_T(other.m_T), m_DT(other.m_DT)
{
	other.m_Data = nullptr;
	m_Characteristic(*this);
}

//Particle& Particle::operator=(const Particle& other)
//{
//	if (this == &other)
//		return *this;
//	m_Shape = std::make_shared<DebugPolygon>(*other.m_Shape);
//	m_Characteristic = other.m_Characteristic;
//	m_LifespanInv = other.m_LifespanInv;
//	m_Invalid = other.m_Invalid;
//	m_T = other.m_T;
//	m_DT = other.m_DT;
//	m_DataSize = other.m_DataSize;
//	if (m_Data)
//		delete[] m_Data;
//	if (m_DataSize > 0)
//	{
//		m_Data = new float[m_DataSize];
//		memcpy_s(m_Data, m_DataSize * sizeof(float), other.m_Data, m_DataSize * sizeof(float));
//	}
//	else
//		m_Data = nullptr;
//	return *this;
//}

Particle& Particle::operator=(Particle&& other) noexcept
{
	if (this == &other)
		return *this;
	m_Shape = std::move(other.m_Shape);
	m_Characteristic = std::move(other.m_Characteristic);
	m_LifespanInv = other.m_LifespanInv;
	m_Invalid = other.m_Invalid;
	m_T = other.m_T;
	m_DT = other.m_DT;
	m_DataSize = other.m_DataSize;
	if (m_Data)
		delete[] m_Data;
	m_Data = other.m_Data;
	other.m_Data = nullptr;
	return *this;
}

Particle::~Particle()
{
	if (m_Data)
		delete[] m_Data;
}

void Particle::OnDraw(real delta_time)
{
	m_DT = delta_time * m_LifespanInv;
	if ((m_T += m_DT) > 1.0f)
		m_Invalid = true;
	else
		m_Characteristic(*this);
}
