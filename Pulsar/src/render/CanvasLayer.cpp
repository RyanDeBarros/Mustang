#include "render/CanvasLayer.h"

#include "Macros.h"
#include "Renderer.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "factory/UniformLexiconFactory.h"

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_LayerView((float)m_Data.pLeft, (float)m_Data.pRight, (float)m_Data.pBottom, (float)m_Data.pTop)
{
	m_VAOs = new std::unordered_map<BatchModel, VAO>();
	m_Batcher = new std::map<ZIndex, std::list<ActorRenderBase2D_P>*>();
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
	if (m_Batcher)
	{
		for (const auto& list : *m_Batcher)
		{
			if (list.second)
				delete list.second;
		}
		m_Batcher->clear();
		delete m_Batcher;
	}
	if (m_VertexPool)
		delete m_VertexPool;
	if (m_IndexPool)
		delete m_IndexPool;

	TRY(glDeleteBuffers(1, &m_VB));
	TRY(glDeleteBuffers(1, &m_IB));

	if (m_VAOs)
	{
		for (const auto& [model, vao] : *m_VAOs)
		{
			TRY(glDeleteVertexArrays(1, &vao));
		}
		delete m_VAOs;
	}
}

void CanvasLayer::OnAttach(ActorPrimitive2D* const primitive)
{
	auto entry = m_Batcher->find(primitive->GetZIndex());
	if (entry == m_Batcher->end())
	{
		m_Batcher->emplace(primitive->GetZIndex(), new std::list<ActorRenderBase2D_P>());
		entry = m_Batcher->find(primitive->GetZIndex());
	}
	entry->second->push_back(primitive);
}

void CanvasLayer::OnAttach(ActorSequencer2D* const composite)
{
	auto entry = m_Batcher->find(composite->GetZIndex());
	if (entry == m_Batcher->end())
	{
		m_Batcher->emplace(composite->GetZIndex(), new std::list<ActorRenderBase2D_P>());
		entry = m_Batcher->find(composite->GetZIndex());
	}
	entry->second->push_back(composite);
}

bool CanvasLayer::OnSetZIndex(ActorPrimitive2D* const primitive, const ZIndex new_val)
{
	if (!OnDetach(primitive))
		return false;
	primitive->m_Z = new_val;
	OnAttach(primitive);
	return true;
}

bool CanvasLayer::OnSetZIndex(ActorSequencer2D* const composite, const ZIndex new_val)
{
	if (!OnDetach(composite))
		return false;
	composite->SetZIndex(new_val);
	OnAttach(composite);
	return true;
}

bool CanvasLayer::OnDetach(ActorPrimitive2D* const primitive)
{
	auto entry = m_Batcher->find(primitive->GetZIndex());
	if (entry == m_Batcher->end())
		return false;
	entry->second->remove(primitive);
	return true;
}

bool CanvasLayer::OnDetach(ActorSequencer2D* const composite)
{
	auto entry = m_Batcher->find(composite->GetZIndex());
	if (entry == m_Batcher->end())
		return false;
	entry->second->remove(composite);
	return true;
}

void CanvasLayer::OnDraw()
{
	SetBlending();
	int prim_i = 0;
	ActorPrimitive2D* primitive = nullptr;
	ActorSequencer2D* sequencer = nullptr;
	currentModel = BatchModel();
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
	for (const auto& list : *m_Batcher)
	{
		for (const auto& element : *list.second)
		{
			if (element.index() == 0)
				DrawPrimitive(std::get<ActorPrimitive2D* const>(element));
			else
			{
				prim_i = 0;
				sequencer = std::get<ActorSequencer2D* const>(element);
				while ((primitive = sequencer->operator[](prim_i++)) != nullptr)
					DrawPrimitive(primitive);
			}
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
			if (m_VAOs->find(currentModel) == m_VAOs->end())
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

inline void CanvasLayer::PoolOver(const Renderable& render)
{
	if (render.vertexBufferData)
		memcpy_s(vertexPos, m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool), render.vertexBufferData, Render::VertexBufferLayoutCount(render) * sizeof(GLfloat));
	if (render.indexBufferData)
		memcpy_s(indexPos, m_Data.maxIndexPoolSize - (indexPos - m_IndexPool), render.indexBufferData, render.indexCount * sizeof(GLuint));
	if (render.vertexCount)
		for (PointerOffset ic = 0; ic < render.indexCount; ic++)
			*(indexPos + ic) += (GLuint)(vertexPos - m_VertexPool) / Render::StrideCountOf(render.model.layout, render.model.layoutMask);
	vertexPos += Render::VertexBufferLayoutCount(render);
	indexPos += render.indexCount;
}

inline TextureSlot CanvasLayer::GetTextureSlot(const Renderable& render)
{
	if (render.textureHandle == 0)
	{
		// no texture
		return -1;
	}
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
	{
		if (*it == render.textureHandle)
		{
			return (TextureSlot)(it - m_TextureSlotBatch.begin());
		}
	}
	if (m_TextureSlotBatch.size() >= _RendererSettings::max_texture_slots)
	{
		FlushAndReset();
	}
	TextureSlot slot = (TextureSlot)m_TextureSlotBatch.size();
	m_TextureSlotBatch.push_back(render.textureHandle);
	return slot;
}

inline void CanvasLayer::FlushAndReset()
{
	if (currentModel == Render::NullModel)
		return;
	TRY(glBindVertexArray((*m_VAOs)[currentModel]));
	BindBuffers();
	TRY(glBufferSubData(GL_ARRAY_BUFFER, 0, (vertexPos - m_VertexPool) * sizeof(GLfloat), m_VertexPool));
	TRY(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (indexPos - m_IndexPool) * sizeof(GLuint), m_IndexPool));
	for (auto it = m_TextureSlotBatch.begin(); it != m_TextureSlotBatch.end(); it++)
	{
		// Note: due to the abstraction of glDrawElements and glBufferSubData behind CanvasLayer, there is currently no need to actually call TextureFactory::Unbind on anything.
		TextureFactory::Bind(*it, (TextureSlot)(it - m_TextureSlotBatch.begin()));
	}
	// TODO collapse into material structure?
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

inline void CanvasLayer::RegisterModel() const
{
	BindBuffers();
	GLuint vao;
	TRY(glGenVertexArrays(1, &vao));
	TRY(glBindVertexArray(vao));
	Render::_AttribLayout(currentModel.layout, currentModel.layoutMask);
	TRY(glBindVertexArray(0));
	UnbindBuffers();
	(*m_VAOs)[currentModel] = vao;
}

inline void CanvasLayer::BindBuffers() const
{
	TRY(glBindBuffer(GL_ARRAY_BUFFER, m_VB));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
}

inline void CanvasLayer::UnbindBuffers() const
{
	TRY(glBindBuffer(GL_ARRAY_BUFFER, 0));
	TRY(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
