#pragma once

#include <GL/glew.h>
#include <map>
#include <list>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include "Typedefs.h"
#include "RendererSettings.h"
#include "ActorRenderBase.h"
#include "LayerView.h"
#include "Renderable.h"
#include "factory/UniformLexicon.h"

struct CanvasLayerData
{
	CanvasIndex ci;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	int pLeft, pRight, pBottom, pTop;
	VertexSize maxVertexPoolSize, maxIndexPoolSize;
	CanvasLayerData(CanvasIndex ci, VertexSize max_vertex_pool_size = _RendererSettings::standard_vertex_pool_size, VertexSize max_index_pool_size = _RendererSettings::standard_index_pool_size)
		: ci(ci), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA),
		pLeft(0), pRight(_RendererSettings::initial_window_width), pBottom(0), pTop(_RendererSettings::initial_window_height),
		maxVertexPoolSize(max_vertex_pool_size), maxIndexPoolSize(max_index_pool_size)
	{}
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
	UniformLexicon currentLexicon;
	UniformLexiconHandle currentLexiconHandle = 0;
	std::vector<TextureHandle> m_TextureSlotBatch;
	std::unordered_map<BatchModel, VAO> m_VAOs;

public:
	CanvasLayer(const CanvasLayerData& data);
	CanvasLayer(const CanvasLayer&) = delete;
	CanvasLayer(CanvasLayer&&) = delete;
	~CanvasLayer();

	void OnAttach(ActorRenderBase2D* const actor);
	bool OnSetZIndex(ActorRenderBase2D* const actor, ZIndex new_val);
	bool OnDetach(ActorRenderBase2D* const actor);
	void OnDraw();

	inline LayerView2D& GetLayerView2DRef() { return m_LayerView; }
	inline CanvasIndex GetZIndex() const { return m_Data.ci; }
	inline CanvasLayerData& GetDataRef() { return m_Data; }

	void DrawPrimitive(class ActorPrimitive2D&);
	void DrawArray(const Renderable& renderable, GLenum indexing_mode);
	void DrawMultiArray(const class DebugMultiPolygon&);

private:
	void SetBlending() const;
	void PoolOver(const Renderable&);
	void PoolOverVerticesOnly(const Renderable&);
	TextureSlot GetTextureSlot(const Renderable&);
	void FlushAndReset();
	void RegisterModel();
	void BindBuffers() const;
	void UnbindBuffers() const;
	void BindAllExceptIndexes();
	void UnbindAll();
};
