#pragma once

typedef unsigned int BufferCounter;
typedef unsigned int VertexSize;
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

#include <variant>
class ActorPrimitive2D;
class ActorSequencer2D;
typedef std::variant<ActorPrimitive2D* const, ActorSequencer2D* const> ActorRenderBase2D_P;

#include "GL/glew.h"
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
