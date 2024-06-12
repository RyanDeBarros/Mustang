#pragma once

typedef unsigned int BufferCounter;
typedef unsigned int VertexSize;
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef unsigned short Stride;

typedef unsigned int ShaderHandle;
typedef unsigned int TextureHandle;
typedef unsigned int MaterialHandle;

typedef signed short ZIndex;
typedef signed char CanvasIndex;

#include <variant>
class ActorPrimitive2D;
class ActorComposite2D;
typedef std::variant<ActorPrimitive2D* const, ActorComposite2D* const> ActorRenderBase2D;

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
