#include "render/CanvasLayer.h"

#include "Macros.h"
#include "Renderer.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "factory/UniformLexiconFactory.h"
#include "actors/ActorPrimitive.h"
#include "actors/ActorSequencer.h"

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_LayerView((float)m_Data.pLeft, (float)m_Data.pRight, (float)m_Data.pBottom, (float)m_Data.pTop)
{
	m_VertexPool = new GLfloat[m_Data.maxVertexPoolSize];
	m_IndexPool = new GLuint[m_Data.maxIndexPoolSize];
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;

	TRY(glGenBuffers(1, &m_VB));
	TRY(glGenBuffers(1, &m_IB));
	BindBuffers();
	TRY(glBufferData(GL_ARRAY_BUFFER, m_Data.maxVertexPoolSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW));
	TRY(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Data.maxIndexPoolSize * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
	UnbindBuffers();
}

CanvasLayer::~CanvasLayer()
{
	for (const auto& list : m_Batcher)
	{
		if (list.second)
			delete list.second;
	}
	m_Batcher.clear();
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

	TRY(glDeleteBuffers(1, &m_VB));
	TRY(glDeleteBuffers(1, &m_IB));
	m_VB = m_IB = 0;

	for (const auto& [model, vao] : m_VAOs)
	{
		TRY(glDeleteVertexArrays(1, &vao));
	}
	m_VAOs.clear();
}

void CanvasLayer::OnAttach(ActorRenderBase2D* const actor)
{
	auto entry = m_Batcher.find(actor->GetZIndex());
	if (entry == m_Batcher.end())
	{
		m_Batcher.emplace(actor->GetZIndex(), new std::list<ActorRenderBase2D*>());
		entry = m_Batcher.find(actor->GetZIndex());
	}
	entry->second->push_back(actor);
}

bool CanvasLayer::OnSetZIndex(ActorRenderBase2D* const actor, const ZIndex new_val)
{
	if (!OnDetach(actor))
		return false;
	actor->SetZIndex(new_val);
	OnAttach(actor);
	return true;
}

bool CanvasLayer::OnDetach(ActorRenderBase2D* const actor)
{
	auto entry = m_Batcher.find(actor->GetZIndex());
	if (entry == m_Batcher.end())
		return false;
	entry->second->remove(actor);
	return true;
}

void CanvasLayer::OnDraw()
{
	SetBlending();
	currentModel = BatchModel();
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
	currentLexicon.Clear();
	for (const auto& list : m_Batcher)
	{
		for (const auto& element : *list.second)
		{
			element->RequestDraw(this);
		}
	}
	FlushAndReset();
}

void CanvasLayer::DrawPrimitive(ActorPrimitive2D* const primitive)
{
	if ((primitive->m_Status & 1) > 0)
	{
		const auto& render = primitive->m_Render;
		if (render.model != currentModel)
		{
			FlushAndReset();
			currentModel = render.model;
			currentLexiconHandle = render.model.uniformLexicon;
			currentLexicon.Clear();
			currentLexicon.MergeLexicon(render.model.uniformLexicon);
			if (m_VAOs.find(currentModel) == m_VAOs.end())
				RegisterModel();
		}
		else if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < Render::VertexBufferLayoutCount(render)
				|| m_Data.maxIndexPoolSize - (indexPos - m_IndexPool) < render.indexCount)
		{
			FlushAndReset();
		}
		primitive->OnDraw(GetTextureSlot(render));
		PoolOver(render);
	}
}

void CanvasLayer::DrawSequencer(ActorSequencer2D* const sequencer)
{
	int prim_i = 0;
	ActorPrimitive2D* primitive = nullptr;
	sequencer->OnPreDraw();
	while ((primitive = sequencer->operator[](prim_i++)) != nullptr)
		DrawPrimitive(primitive);
	sequencer->OnPostDraw();
}

void CanvasLayer::SetBlending() const
{
	if (m_Data.enableGLBlend)
	{
		TRY(glEnable(GL_BLEND));
		TRY(glBlendFunc(m_Data.sourceBlend, m_Data.destBlend));
	}
	else
	{
		TRY(glDisable(GL_BLEND));
	}
}

void CanvasLayer::PoolOver(const Renderable& render)
{
	if (render.vertexBufferData)
		memcpy_s(vertexPos, (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool)) * sizeof(VertexSize), render.vertexBufferData, Render::VertexBufferLayoutCount(render) * sizeof(GLfloat));
	if (render.indexBufferData)
		memcpy_s(indexPos, (m_Data.maxIndexPoolSize - (indexPos - m_IndexPool)) * sizeof(VertexSize), render.indexBufferData, render.indexCount * sizeof(GLuint));
	if (render.vertexCount)
		for (PointerOffset ic = 0; ic < render.indexCount; ic++)
			*(indexPos + ic) += (GLuint)(vertexPos - m_VertexPool) / Render::StrideCountOf(render.model.layout, render.model.layoutMask);
	vertexPos += Render::VertexBufferLayoutCount(render);
	indexPos += render.indexCount;
	if (render.model.uniformLexicon != currentLexiconHandle)
		currentLexicon.MergeLexicon(render.model.uniformLexicon);
}

TextureSlot CanvasLayer::GetTextureSlot(const Renderable& render)
{
	if (render.textureHandle == 0)
	{
		// no texture
		return -1;
	}
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
	{
		if (*it == render.textureHandle)
			return static_cast<TextureSlot>(it - m_TextureSlotBatch.begin());
	}
	if (m_TextureSlotBatch.size() >= _RendererSettings::max_texture_slots)
	{
		FlushAndReset();
	}
	TextureSlot slot = static_cast<TextureSlot>(m_TextureSlotBatch.size());
	m_TextureSlotBatch.push_back(render.textureHandle);
	return slot;
}

void CanvasLayer::FlushAndReset()
{
	if (currentModel == BatchModel{})
		return;
	TRY(glBindVertexArray(m_VAOs[currentModel]));
	BindBuffers();
	TRY(glBufferSubData(GL_ARRAY_BUFFER, 0, (vertexPos - m_VertexPool) * sizeof(GLfloat), m_VertexPool));
	TRY(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (indexPos - m_IndexPool) * sizeof(GLuint), m_IndexPool));
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
	{
		// Note: due to the abstraction of glDrawElements and glBufferSubData behind CanvasLayer, there is currently no need to actually call TextureFactory::Unbind on anything.
		TextureFactory::Bind(*it, (TextureSlot)(it - m_TextureSlotBatch.begin()));
	}
	ShaderFactory::Bind(currentModel.shader);
	m_LayerView.PassVPUniform(currentModel.shader);
	UniformLexiconFactory::OnApply(currentModel.uniformLexicon, currentModel.shader);
	TRY(glDrawElements(GL_TRIANGLES, (GLsizei)(indexPos - m_IndexPool), GL_UNSIGNED_INT, nullptr));
	ShaderFactory::Unbind();
	UnbindBuffers();
	TRY(glBindVertexArray(0));
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
	m_TextureSlotBatch.clear();
}

void CanvasLayer::RegisterModel()
{
	BindBuffers();
	GLuint vao;
	TRY(glGenVertexArrays(1, &vao));
	TRY(glBindVertexArray(vao));
	Render::_AttribLayout(currentModel.layout, currentModel.layoutMask);
	TRY(glBindVertexArray(0));
	UnbindBuffers();
	m_VAOs[currentModel] = vao;
}

void CanvasLayer::BindBuffers() const
{
	TRY(glBindBuffer(GL_ARRAY_BUFFER, m_VB));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
}

void CanvasLayer::UnbindBuffers() const
{
	TRY(glBindBuffer(GL_ARRAY_BUFFER, 0));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
