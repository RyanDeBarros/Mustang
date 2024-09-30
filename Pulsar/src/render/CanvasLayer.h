#pragma once

#include <GL/glew.h>
#include <map>
#include <list>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include "PulsarSettings.h"
#include "utils/Functor.inl"
#include "ActorRenderBase.h"
#include "LayerView.h"
#include "Renderable.h"
#include "registry/UniformLexicon.h"

typedef signed char CanvasIndex;

struct CanvasLayerData
{
	CanvasIndex ci;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	int pLeft, pRight, pBottom, pTop;
	VertexSize maxVertexPoolSize, maxIndexPoolSize;
	CanvasLayerData(CanvasIndex ci, VertexSize max_vertex_pool_size = 0, VertexSize max_index_pool_size = 0)
		: ci(ci), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA),
		pLeft(0), pRight(PulsarSettings::initial_window_width()), pBottom(0), pTop(PulsarSettings::initial_window_height()),
		maxVertexPoolSize(max_vertex_pool_size > 0 ? max_vertex_pool_size : PulsarSettings::standard_vertex_pool_size()),
		maxIndexPoolSize(max_index_pool_size > 0 ? max_index_pool_size : PulsarSettings::standard_index_pool_size())
	{}
};

typedef GLuint VAO;

enum class DrawMode : unsigned char
{
	VOID,
	PRIMITIVE,
	ARRAY,
	MULTI_ARRAY,
	RECT
};

class RectBatcher
{
	friend class CanvasLayer;

	std::vector<GLint> indexes; // { 0, 4, 8, ... }
	std::vector<GLsizei> index_counts; // { 4, 4, 4, ... }
	GLsizei draw_count = 0;
	GLsizei size = 0;

	void set_size(GLsizei i);
	bool increment_and_push_size(GLsizei hard_limit, GLsizei hit_limit_incr = 25);
	bool try_increment();
};

class CanvasLayer
{
	friend class Renderer;
	CanvasLayerData m_Data;
	LayerView2D m_LayerView;
	std::map<ZIndex, std::list<ActorRenderBase2D*>> m_Batcher;
	GLfloat* m_VertexPool;
	GLfloat* vertexPos;
	GLuint* m_IndexPool;
	GLuint* indexPos;
	GLuint m_VB, m_IB;
	BatchModel currentModel;
	DrawMode currentDrawMode = DrawMode::VOID;
	UniformLexicon currentLexicon;
	std::vector<TextureHandle> m_TextureSlotBatch;
	std::unordered_map<BatchModel, VAO> m_VAOs;
	RectBatcher rectBatcher;

public:
	CanvasLayer(const CanvasLayerData& data);
	CanvasLayer(const CanvasLayer&) = delete;
	CanvasLayer(CanvasLayer&&) = delete;
	~CanvasLayer();

	void OnAttach(ActorRenderBase2D* const actor);
	bool OnSetZIndex(ActorRenderBase2D* const actor, ZIndex new_val);
	bool OnDetach(ActorRenderBase2D* const actor);
	void Clear();
	void OnDraw();

	LayerView2D& GetLayerView2DRef() { return m_LayerView; }
	CanvasIndex GetZIndex() const { return m_Data.ci; }
	CanvasLayerData& GetDataRef() { return m_Data; }

	void DrawPrimitive(class ActorPrimitive2D*);
	void DrawArray(const Renderable& renderable, GLenum indexing_mode);
	void DrawMultiArray(class DebugMultiPolygon*);
	void DrawRect(const Renderable& renderable, const Functor<void, TextureSlot>& on_draw_callback);

private:
	void SetBlending() const;
	void SetBatchModel(const BatchModel&);
	void SetUniformLexicon(UniformLexiconHandle lexicon);
	void PoolOverAll(const Renderable&);
	void PoolOverIndexBuffer(const Renderable&);
	void PoolOverVertexBuffer(const Renderable&);
	void PoolOverLexicon(UniformLexiconHandle lexicon);
	void FlushAndReset();
	TextureSlot GetTextureSlot(const Renderable&);
	
	void RegisterModel();
	void BindVertexArray(GLuint) const;
	void UnbindVertexArray() const;
	void OpenShading() const;
	void CloseShading() const;
	void ResetPoolsAndLexicon();
	void BindTextureSlots() const;
	void SendVertexPool() const;
	void SendIndexPool() const;

	void SendTriangles();
	void SendArray(const Renderable& renderable, GLenum indexing_mode);
	void SendMultiArray(class DebugMultiPolygon*);
	void SendRects();
};
