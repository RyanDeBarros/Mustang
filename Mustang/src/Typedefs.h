#pragma once

typedef unsigned int VertexCounter;
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef signed short ZIndex;
typedef signed char CanvasIndex;
typedef unsigned int ShaderHandle;

#include "GL/glew.h"
typedef GLuint RID;
typedef GLint TextureSlot;

#ifdef WIN32
	typedef signed int ActorPrimitiveCounter;
#else
	typedef signed long long ActorPrimitiveCounter;
#endif
