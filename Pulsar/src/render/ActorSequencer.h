#pragma once

#include "Typedefs.h"

class ActorSequencer2D
{
	friend class CanvasLayer;
	virtual void SetZIndex(const ZIndex& z) = 0;
public:
	virtual ActorPrimitive2D* operator[](const int& i) = 0;
	virtual ZIndex GetZIndex() const = 0;
};
