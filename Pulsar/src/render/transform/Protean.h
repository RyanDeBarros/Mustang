#pragma once

#include "Transform.h"
#include "Modulate.h"

class Transformable2D
{
protected:
	Transformer2D m_Transformer;

public:
	inline Transformable2D(const Transform2D& transform = {}) : m_Transformer(transform) {}

	inline Transformer2D* RefTransformer() { return &m_Transformer; }
	inline Transform2D* RefTransform() { return &m_Transformer.self.transform; }
};

class Modulatable
{
protected:
	Modulator m_Modulator;

public:
	inline Modulatable(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : m_Modulator(modulate) {}

	inline Modulator* RefModuator() { return &m_Modulator; }
	inline Modulate* RefModulate() { return &m_Modulator.self.modulate; }
};

class Protean2D : public Transformable2D, public Modulatable
{
public:
	inline Protean2D(const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : Transformable2D(transform), Modulatable(modulate) {}
};
