#pragma once

typedef unsigned int VertexCounter;

#ifdef WIN32
typedef signed int ActorPrimitiveCounter;
#else
typedef signed long long ActorPrimitiveCounter;
#endif
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef signed short ZIndex;
typedef unsigned int ShaderHandle;
