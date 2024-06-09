#pragma once

#include <GL/glew.h>
#include <map>
#include <list>
#include <variant>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RendererSettings.h"
#include "Typedefs.h"
#include "Transform.h"
#include "ActorPrimitive.h"
#include "ActorComposite.h"
#include "Renderable.h"

struct CanvasLayerData
{
	CanvasIndex ci;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	float pLeft, pRight, pBottom, pTop;
	VertexSize maxVertexPoolSize, maxIndexPoolSize;
	CanvasLayerData(CanvasIndex ci, VertexSize max_vertex_pool_size, VertexSize max_index_pool_size)
		: ci(ci), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA), pLeft(0), pRight(_RendererSettings::window_width), pBottom(0), pTop(_RendererSettings::window_height),
		maxVertexPoolSize(max_vertex_pool_size), maxIndexPoolSize(max_index_pool_size)
	{}
};

class CanvasLayer
{
	CanvasLayerData m_Data;
	glm::mat3 m_Proj;
	Transform2D m_CameraTransform;
	glm::mat3 m_View;
	std::map<ZIndex, std::list<ActorRenderBase2D>*>* m_Batcher;
	GLfloat* m_VertexPool;
	GLfloat* vertexPos;
	GLuint* m_IndexPool;
	GLuint* indexPos;
	GLuint m_VB, m_IB;
	BatchModel currentModel;
	std::vector<TextureHandle> m_TextureSlotBatch;
public:
	CanvasLayer(CanvasLayerData data);
	~CanvasLayer();

	void OnAttach(ActorPrimitive2D* const primitive);
	void OnAttach(ActorComposite2D* const composite);
	bool OnSetZIndex(ActorPrimitive2D* const primitive, const ZIndex new_val);
	bool OnSetZIndex(ActorComposite2D* const composite, const ZIndex new_val);
	bool OnDetach(ActorPrimitive2D* const primitive);
	bool OnDetach(ActorComposite2D* const composite);
	void OnDraw();

	CanvasIndex getZIndex() const { return m_Data.ci; }
	CanvasLayerData& getDataRef() { return m_Data; }
private:
	inline void SetBlending() const;
	inline void PoolOver(const Renderable& render);
	inline TextureSlot GetTextureSlot(const Renderable&);
	inline void FlushAndReset();
	inline void RegisterModel() const;
	inline void BindBuffers() const;
	inline void UnbindBuffers() const;
};
