#pragma once

#include "Transform.h"
#include "Modulate.h"
#include "Protean.h"

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

	inline Modulator* RefModulator() { return &m_Modulator; }
	inline Modulate* RefModulate() { return &m_Modulator.self.modulate; }
};

struct Protean2D
{
protected:
	ProteanLinker2D m_ProteanLinker;

public:
	inline Protean2D(const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : m_ProteanLinker(transform, modulate) {}

	inline ProteanLinker2D* RefProteanLinker() { return &m_ProteanLinker; }
	inline Transform2D* RefTransform() { return &m_ProteanLinker.self.proteate.transform; }
	inline Modulate* RefModulate() { return &m_ProteanLinker.self.proteate.modulate; }
};
