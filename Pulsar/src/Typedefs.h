#pragma once

typedef unsigned int BufferCounter;
typedef BufferCounter VertexSize;
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef unsigned short Stride;

typedef unsigned short ShaderHandle;
typedef unsigned short TextureHandle;
typedef unsigned short UniformLexiconHandle;
typedef unsigned short TileHandle;
typedef unsigned short TextureVersion;

typedef signed short ZIndex;
typedef signed char CanvasIndex;

typedef int AtlasPos;

#include <GL/glew.h>
typedef GLuint RID;
typedef GLuint VAO;
typedef GLint TextureSlot;

#ifdef WIN32
typedef signed int ActorCounter;
typedef signed int PointerOffset;
#else
typedef signed long long ActorCounter;
typedef signed long long PointerOffset;
#endif

#include "Macros.h"
// TODO rename real to something more intuitive. Currently, it is used for time and delta time values.
#if PULSAR_DELTA_USE_DOUBLE_PRECISION == 1
typedef double real;
#else
typedef float real;
#endif
