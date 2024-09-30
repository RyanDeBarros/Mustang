#include "render/CanvasLayer.h"

#include "Macros.h"
#include "Renderer.h"
#include "registry/Shader.h"
#include "actors/ActorPrimitive.h"
#include "actors/shapes/DebugMultiPolygon.h"
#include "actors/RectRender.h"

void RectBatcher::set_size(GLsizei i)
{
	GLsizei prev_size = static_cast<GLsizei>(indexes.size());
	if (i < prev_size)
	{
		indexes.resize(i);
		index_counts.resize(i);
		indexes.shrink_to_fit();
		index_counts.shrink_to_fit();
	}
	else
	{
		indexes.resize(i);
		index_counts.resize(i);
		for (size_t j = prev_size; j < i; ++j)
		{
			indexes[j] = static_cast<GLint>(4 * j);
			index_counts[j] = static_cast<GLsizei>(4);
		}
	}
	size = i;
}

bool RectBatcher::increment_and_push_size(GLsizei hard_limit, GLsizei hit_limit_incr)
{
	if (draw_count + 1 < size) [[likely]]
	{
		++draw_count;
		return true;
	}
	else if (size + hit_limit_incr <= hard_limit)
	{
		set_size(size + hit_limit_incr);
		++draw_count;
		return true;
	}
	else return false;
}

bool RectBatcher::try_increment()
{
	if (draw_count + 1 < size) [[likely]]
	{
		++draw_count;
		return true;
	}
	else return false;
}

CanvasLayer::CanvasLayer(const CanvasLayerData& data)
	: m_Data(data), m_LayerView((float)m_Data.pLeft, (float)m_Data.pRight, (float)m_Data.pBottom, (float)m_Data.pTop)
{
	m_VertexPool = new GLfloat[m_Data.maxVertexPoolSize];
	m_IndexPool = new GLuint[m_Data.maxIndexPoolSize];
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;

	// generate buffers
	PULSAR_TRY(glGenBuffers(1, &m_VB));
	PULSAR_TRY(glGenBuffers(1, &m_IB));
	// bind buffers
	PULSAR_TRY(glBindBuffer(GL_ARRAY_BUFFER, m_VB));
	PULSAR_TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	// initialize buffers
	PULSAR_TRY(glBufferData(GL_ARRAY_BUFFER, m_Data.maxVertexPoolSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW));
	PULSAR_TRY(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Data.maxIndexPoolSize * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
	// unbind buffers
	PULSAR_TRY(glBindBuffer(GL_ARRAY_BUFFER, 0));
	PULSAR_TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

CanvasLayer::~CanvasLayer()
{
	if (m_VertexPool)
	{
		delete m_VertexPool;
		m_VertexPool = nullptr;
	}
	if (m_IndexPool)
	{
		delete m_IndexPool;
		m_IndexPool = nullptr;
	}

	PULSAR_TRY(glDeleteBuffers(1, &m_VB));
	PULSAR_TRY(glDeleteBuffers(1, &m_IB));
	m_VB = m_IB = 0;

	for (const auto& [model, vao] : m_VAOs)
	{
		PULSAR_TRY(glDeleteVertexArrays(1, &vao));
	}
}

void CanvasLayer::OnAttach(ActorRenderBase2D* const actor)
{
	auto entry = m_Batcher.find(actor->z);
	if (entry == m_Batcher.end())
	{
		m_Batcher.emplace(actor->z, std::list<ActorRenderBase2D*>());
		entry = m_Batcher.find(actor->z);
	}
	entry->second.push_back(actor);
}

bool CanvasLayer::OnSetZIndex(ActorRenderBase2D* const actor, ZIndex new_val)
{
	if (!OnDetach(actor))
		return false;
	actor->z = new_val;
	OnAttach(actor);
	return true;
}

bool CanvasLayer::OnDetach(ActorRenderBase2D* const actor)
{
	auto entry = m_Batcher.find(actor->z);
	if (entry == m_Batcher.end())
		return false;
	entry->second.remove(actor);
	return true;
}

void CanvasLayer::Clear()
{
	for (const auto& [model, vao] : m_VAOs)
	{
		PULSAR_TRY(glDeleteVertexArrays(1, &vao));
	}
	m_VAOs.clear();
	m_TextureSlotBatch.clear();
	m_Batcher.clear();
	rectBatcher.set_size(0);
	rectBatcher.draw_count = 0;
	ResetPoolsAndLexicon();
}

void CanvasLayer::OnDraw()
{
	SetBlending();
	currentModel = BatchModel();
	ResetPoolsAndLexicon();
	for (const auto& list : m_Batcher)
		for (const auto& element : list.second)
			element->RequestDraw(this);
	FlushAndReset();
}

void CanvasLayer::DrawPrimitive(ActorPrimitive2D* primitive)
{
	if (currentDrawMode != DrawMode::PRIMITIVE)
	{
		FlushAndReset();
		currentDrawMode = DrawMode::PRIMITIVE;
	}
	const auto& render = primitive->m_Render;
	if (render.model != currentModel || !currentLexicon.Shares(render.uniformLexicon))
	{
		SendTriangles();
		SetBatchModel(render.model);
		SetUniformLexicon(render.uniformLexicon);
	}
	else if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < Render::VertexBufferLayoutCount(render)
			|| m_Data.maxIndexPoolSize - (indexPos - m_IndexPool) < render.indexCount)
	{
		SendTriangles();
	}
	primitive->OnDraw(GetTextureSlot(render));
	PoolOverAll(render);
}

void CanvasLayer::DrawArray(const Renderable& renderable, GLenum indexing_mode)
{
	FlushAndReset();
	currentDrawMode = DrawMode::ARRAY;
	SetBatchModel(renderable.model);
	SetUniformLexicon(renderable.uniformLexicon);
	PoolOverVertexBuffer(renderable);
	PoolOverLexicon(renderable.uniformLexicon);
	SendArray(renderable, indexing_mode);
}

void CanvasLayer::DrawMultiArray(DebugMultiPolygon* multi_polygon)
{
	FlushAndReset();
	currentDrawMode = DrawMode::MULTI_ARRAY;
	SetBatchModel(multi_polygon->m_Model);
	SetUniformLexicon(0);
	for (const auto& poly : multi_polygon->m_Polygons)
	{
		if (!poly->DrawPrep())
			continue;
		if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < Render::VertexBufferLayoutCount(poly->m_Renderable))
			SendMultiArray(multi_polygon);
		PoolOverVertexBuffer(poly->m_Renderable);
		PoolOverLexicon(poly->m_Renderable.uniformLexicon);
	}
	SendMultiArray(multi_polygon);
}

void CanvasLayer::DrawRect(const Renderable& renderable, const Functor<void, TextureSlot>& on_draw_callback)
{
	if (currentDrawMode != DrawMode::RECT)
	{
		FlushAndReset();
		currentDrawMode = DrawMode::RECT;
	}
	if (!rectBatcher.increment_and_push_size(m_Data.maxIndexPoolSize))
	{
		SendRects();
		rectBatcher.draw_count = 1;
	}
	if (renderable.model != currentModel || !currentLexicon.Shares(renderable.uniformLexicon))
	{
		SendRects();
		rectBatcher.draw_count = 1;
		SetBatchModel(renderable.model);
		SetUniformLexicon(renderable.uniformLexicon);
	}
	else if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < Render::VertexBufferLayoutCount(renderable)
		|| m_Data.maxIndexPoolSize - (indexPos - m_IndexPool) < renderable.indexCount)
	{
		SendRects();
		rectBatcher.draw_count = 1;
	}
	on_draw_callback(GetTextureSlot(renderable));
	PoolOverVertexBuffer(renderable);
	PoolOverLexicon(renderable.uniformLexicon);
}

void CanvasLayer::SetBlending() const
{
	if (m_Data.enableGLBlend)
	{
		PULSAR_TRY(glEnable(GL_BLEND));
		PULSAR_TRY(glBlendFunc(m_Data.sourceBlend, m_Data.destBlend));
	}
	else
	{
		PULSAR_TRY(glDisable(GL_BLEND));
	}
}

void CanvasLayer::SetBatchModel(const BatchModel& model)
{
	currentModel = model;
	if (m_VAOs.find(currentModel) == m_VAOs.end())
		RegisterModel();
}

void CanvasLayer::SetUniformLexicon(UniformLexiconHandle lexicon)
{
	currentLexicon.Clear();
	currentLexicon.MergeLexicon(lexicon);
}

// NOTE If buffer data is too large to fit in corresponding pool, it will not be rendered.
// This check would have to be done before pooling over, in which case FlushAndReset() can be called.
void CanvasLayer::PoolOverAll(const Renderable& renderable)
{
	// order of these calls is crucial
	PoolOverIndexBuffer(renderable);
	PoolOverVertexBuffer(renderable);
	PoolOverLexicon(renderable.uniformLexicon);
}

void CanvasLayer::PoolOverIndexBuffer(const Renderable& renderable)
{
	if (renderable.indexBufferData)
		memcpy_s(indexPos, (m_Data.maxIndexPoolSize - (indexPos - m_IndexPool)) * sizeof(VertexSize),
			renderable.indexBufferData, renderable.indexCount * sizeof(GLuint));
	if (renderable.vertexCount)
		for (size_t ic = 0; ic < renderable.indexCount; ic++)
			*(indexPos + ic) += (GLuint)(vertexPos - m_VertexPool) / Render::StrideCountOf(renderable.model.layout, renderable.model.layoutMask);
	indexPos += renderable.indexCount;
}

void CanvasLayer::PoolOverVertexBuffer(const Renderable& renderable)
{
	if (renderable.vertexBufferData)
		memcpy_s(vertexPos, (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool)) * sizeof(VertexSize),
			renderable.vertexBufferData, Render::VertexBufferLayoutCount(renderable) * sizeof(GLfloat));
	vertexPos += Render::VertexBufferLayoutCount(renderable);
}

void CanvasLayer::PoolOverLexicon(UniformLexiconHandle lexicon)
{
	currentLexicon.MergeLexicon(lexicon);
}

void CanvasLayer::FlushAndReset()
{
	if (currentDrawMode == DrawMode::RECT) [[likely]]
		SendRects();
	else if (currentDrawMode == DrawMode::PRIMITIVE)
		SendTriangles();
}

TextureSlot CanvasLayer::GetTextureSlot(const Renderable& render)
{
	if (render.textureHandle == 0) // no texture
		return -1;
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
	{
		if (*it == render.textureHandle)
			return static_cast<TextureSlot>(it - m_TextureSlotBatch.begin());
	}
	if (m_TextureSlotBatch.size() >= PulsarSettings::max_texture_slots())
		FlushAndReset();
	TextureSlot slot = static_cast<TextureSlot>(m_TextureSlotBatch.size());
	m_TextureSlotBatch.push_back(render.textureHandle);
	return slot;
}

void CanvasLayer::RegisterModel()
{
	GLuint vao;
	PULSAR_TRY(glGenVertexArrays(1, &vao));
	BindVertexArray(vao);
	Render::_AttribLayout(currentModel.layout, currentModel.layoutMask);
	UnbindVertexArray();
	m_VAOs[currentModel] = vao;
}

void CanvasLayer::BindVertexArray(GLuint vao) const
{
	// order of these calls is crucial
	PULSAR_TRY(glBindVertexArray(vao));
	PULSAR_TRY(glBindBuffer(GL_ARRAY_BUFFER, m_VB));
	PULSAR_TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
}

void CanvasLayer::UnbindVertexArray() const
{
	// order of these calls is crucial
	PULSAR_TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	PULSAR_TRY(glBindBuffer(GL_ARRAY_BUFFER, 0));
	PULSAR_TRY(glBindVertexArray(0));
}

void CanvasLayer::OpenShading() const
{
	// order of these calls is crucial
	BindVertexArray(m_VAOs.find(currentModel)->second);
	Renderer::Shaders().Bind(currentModel.shader);
	m_LayerView.PassVPUniform(currentModel.shader);
	currentLexicon.OnApply(currentModel.shader);
}

void CanvasLayer::CloseShading() const
{
	Renderer::Shaders().Unbind();
	UnbindVertexArray();
}

void CanvasLayer::ResetPoolsAndLexicon()
{
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
	currentLexicon.Clear();
}

void CanvasLayer::BindTextureSlots() const
{
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
		// NOTE due to the abstraction of glDrawElements and glBufferSubData behind CanvasLayer, there is currently no need to actually call TextureRegistry::Unbind on anything.
		Renderer::Textures().Bind(*it, (TextureSlot)(it - m_TextureSlotBatch.begin()));
}

void CanvasLayer::SendVertexPool() const
{
	PULSAR_TRY(glBufferSubData(GL_ARRAY_BUFFER, 0, (vertexPos - m_VertexPool) * sizeof(GLfloat), m_VertexPool));
}

void CanvasLayer::SendIndexPool() const
{
	PULSAR_TRY(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (indexPos - m_IndexPool) * sizeof(GLuint), m_IndexPool));
}

void CanvasLayer::SendTriangles()
{
	if (vertexPos - m_VertexPool > 0)
	{
		PULSAR_ASSERT(indexPos - m_IndexPool > 0);
		OpenShading();
		BindTextureSlots();
		SendVertexPool();
		SendIndexPool();
		PULSAR_TRY(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexPos - m_IndexPool), GL_UNSIGNED_INT, nullptr));
		CloseShading();
		ResetPoolsAndLexicon();
		m_TextureSlotBatch.clear();
	}
}

void CanvasLayer::SendArray(const Renderable& renderable, GLenum indexing_mode)
{
	if (vertexPos - m_VertexPool > 0)
	{
		OpenShading();
		SendVertexPool();
		PULSAR_TRY(glDrawArrays(indexing_mode, 0, renderable.vertexCount));
		CloseShading();
		ResetPoolsAndLexicon();
	}
}

void CanvasLayer::SendMultiArray(DebugMultiPolygon* multi_polygon)
{
	if (vertexPos - m_VertexPool > 0)
	{
		OpenShading();
		SendVertexPool();
		PULSAR_TRY(glMultiDrawArrays(multi_polygon->m_IndexMode, multi_polygon->indexes_ptr, multi_polygon->index_counts_ptr, multi_polygon->DrawCount()));
		CloseShading();
		ResetPoolsAndLexicon();
	}
}

void CanvasLayer::SendRects()
{
	if (vertexPos - m_VertexPool > 0)
	{
		OpenShading();
		BindTextureSlots();
		SendVertexPool();
		PULSAR_TRY(glMultiDrawArrays(GL_TRIANGLE_FAN, rectBatcher.indexes.data(), rectBatcher.index_counts.data(), rectBatcher.draw_count));
		rectBatcher.draw_count = 0;
		CloseShading();
		ResetPoolsAndLexicon();
		m_TextureSlotBatch.clear();
	}
}
