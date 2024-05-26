#pragma once

typedef unsigned int VertexCounter;
typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;

typedef unsigned int ShaderHandle;
typedef unsigned int TextureHandle;

typedef signed short ZIndex;
typedef signed char CanvasIndex;

#include <variant>
class ActorPrimitive2D;
class ActorComposite2D;
typedef std::variant<ActorPrimitive2D*, ActorComposite2D*> ActorRenderBase2D;

#include "GL/glew.h"
typedef GLuint RID;
typedef GLuint VAO;
typedef GLint TextureSlot;

#ifdef WIN32
	typedef signed int ActorPrimitiveCounter;
#else
	typedef signed long long ActorPrimitiveCounter;
#endif
