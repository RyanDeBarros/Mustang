#include "render/CanvasLayer.h"

#include "Utility.h"
#include "ActorRenderIterator.h"
#include "Renderer.h"
#include "ShaderFactory.h"

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<ActorRenderBase2D>*>();
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
}

void CanvasLayer::OnAttach(ActorPrimitive2D* const primitive)
{
	auto entry = m_Batcher->find(primitive->GetZIndex());
	if (entry == m_Batcher->end())
	{
		m_Batcher->emplace(primitive->GetZIndex(), new std::list<ActorRenderBase2D>());
		entry = m_Batcher->find(primitive->GetZIndex());
	}
	entry->second->push_back(primitive);
}

void CanvasLayer::OnAttach(ActorComposite2D* const composite)
{
	auto entry = m_Batcher->find(composite->GetZIndex());
	if (entry == m_Batcher->end())
	{
		m_Batcher->emplace(composite->GetZIndex(), new std::list<ActorRenderBase2D>());
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

bool CanvasLayer::OnSetZIndex(ActorComposite2D* const composite, const ZIndex new_val)
{
	if (!OnDetach(composite))
		return false;
	composite->m_CompositeZ = new_val;
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

bool CanvasLayer::OnDetach(ActorComposite2D* const composite)
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
	ActorRenderIterator iter;
	currentModel = BatchModel();
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
	for (const auto& list : *m_Batcher)
	{
		for (const auto& element : *list.second)
		{
			iter = std::visit([](auto&& arg) { return ActorRenderIterator(arg); }, element);
			while (*iter)
			{
				if (!(*iter)->m_Visible)
				{
					++iter;
					continue;
				}
				const auto& render = (*iter)->m_Render;
				if (render.model != currentModel)
				{
					FlushAndReset();
					currentModel = render.model;
					if (Renderer::rvaos->find(currentModel) == Renderer::rvaos->end())
					{
						RegisterModel();
					}
				}
				else if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < Render::VertexBufferLayoutCount(render)
						|| m_Data.maxIndexPoolSize - (indexPos - m_IndexPool) < render.indexCount * sizeof(GLuint))
				{
					FlushAndReset();
				}
				(*iter)->OnDraw(GetTextureSlot(render));
				PoolOver(render);
				++iter;
			}
		}
	}
	FlushAndReset();
}

inline void CanvasLayer::SetBlending() const
{
	if (m_Data.enableGLBlend)
	{
		TRY(glEnable(GL_BLEND));
		TRY(glBlendFunc(m_Data.sourceBlend, m_Data.destBlend));
	}
	else
		TRY(glDisable(GL_BLEND));
}

inline void CanvasLayer::PoolOver(const Renderable& render)
{
	memcpy_s(vertexPos, m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool), render.vertexBufferData, Render::VertexBufferLayoutCount(render) * sizeof(GLfloat));
	memcpy_s(indexPos, m_Data.maxIndexPoolSize - (indexPos - m_IndexPool), render.indexBufferData, render.indexCount * sizeof(GLuint));
	for (PointerOffset ic = 0; ic < render.indexCount; ic++)
		*(indexPos + ic) += (vertexPos - m_VertexPool) / Render::VertexBufferLayoutCount(render);
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
			return it - m_TextureSlotBatch.begin();
		}
	}
	if (m_TextureSlotBatch.size() >= RenderSettings::max_texture_slots)
	{
		FlushAndReset();
	}
	m_TextureSlotBatch.push_back(render.textureHandle);
	return m_TextureSlotBatch.size() - 1;
}

inline void CanvasLayer::FlushAndReset()
{
	if (currentModel == Render::NullModel)
		return;
	TRY(glBindVertexArray((*Renderer::rvaos)[currentModel]));
	BindBuffers();
	TRY(glBufferSubData(GL_ARRAY_BUFFER, 0, (vertexPos - m_VertexPool) * sizeof(GLfloat), m_VertexPool));
	TRY(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (indexPos - m_IndexPool) * sizeof(GLuint), m_IndexPool));
	UnbindBuffers();
	ShaderFactory::Bind(currentModel.shader);
	TRY(glDrawElements(GL_TRIANGLES, indexPos - m_IndexPool, GL_UNSIGNED_INT, &m_IndexPool));
	ShaderFactory::Unbind();
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

	// TODO Extract into separate function from Render namespace, just like with StrideCountOf?
	unsigned short offset = 0;
	unsigned char num_attribs = 0;
	while (currentModel.layoutMask >> num_attribs != 0)
	{
		TRY(glEnableVertexAttribArray(num_attribs));
		auto shift = 2 * num_attribs;
		unsigned char attrib = ((currentModel.layout & (3 << shift)) >> shift) + 1;
		TRY(glVertexAttribPointer(num_attribs, attrib, GL_FLOAT, GL_FALSE, Render::StrideCountOf(currentModel.layout, currentModel.layoutMask) * sizeof(GLfloat), (const GLvoid*)offset));
		offset += attrib * sizeof(GLfloat);
		num_attribs++;
	}

	TRY(glBindVertexArray(0));
	UnbindBuffers();
	(*Renderer::rvaos)[currentModel] = vao;
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
