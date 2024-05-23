#pragma once

#include "Typedefs.h"
#include "ActorPrimitive.h"

class ActorConstruct2D
{
	ActorPrimitive2D* m_Root;
	ActorPrimitiveCounter m_NumPrimitives;
	ZIndex m_ConstructZ;
public:
	ActorConstruct2D(ActorPrimitiveCounter num_primitives, ZIndex z = 0);
	~ActorConstruct2D();
};
