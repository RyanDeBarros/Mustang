#pragma once

typedef unsigned int BufferCounter;
typedef BufferCounter VertexSize;
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef unsigned short Stride;

typedef unsigned int ShaderHandle;
typedef unsigned int TextureHandle;
typedef unsigned int UniformLexiconHandle;
typedef unsigned int TileHandle;

typedef signed short ZIndex;
typedef signed char CanvasIndex;

typedef int AtlasPos;

#include <GL/glew.h>
typedef GLuint RID;
typedef GLuint VAO;
typedef GLint TextureSlot;

#ifdef WIN32
typedef signed int ActorPrimitiveCounter;
typedef signed int PointerOffset;
#else
typedef signed long long ActorPrimitiveCounter;
typedef signed long long PointerOffset;
#endif

#include "Macros.h"
#if PULSAR_DELTA_USE_DOUBLE_PRECISION == 1
typedef double real;
#else
typedef float real;
#endif
