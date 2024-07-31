#pragma once

#include "Transformable.h"
#include "Modulatable.h"

class ProteanProxy2D : public TransformableProxy2D, public ModulatableProxy
{
public:
	ProteanProxy2D(const Transform2D& transform = {}, const glm::vec4& color = {});
	ProteanProxy2D(const ProteanProxy2D&);
	ProteanProxy2D(ProteanProxy2D&&) noexcept;
	ProteanProxy2D& operator=(const ProteanProxy2D&);
	ProteanProxy2D& operator=(ProteanProxy2D&&) noexcept;
};
