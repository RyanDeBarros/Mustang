#include "render/ActorConstruct.h"

ActorConstruct2D::ActorConstruct2D(ActorPrimitiveCounter num_primitives, ZIndex z)
	: m_NumPrimitives(num_primitives), m_ConstructZ(z)
{
	m_Root = new ActorPrimitive2D[m_NumPrimitives];
}

ActorConstruct2D::~ActorConstruct2D()
{
	delete[] m_Root;
}
