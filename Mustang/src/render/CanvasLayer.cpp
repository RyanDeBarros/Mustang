#include "render/CanvasLayer.h"

#include "Utility.h"
#include "ActorRenderIterator.h"
#include "Renderer.h"

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<ActorRenderBase2D>*>();
	m_VertexPool = new GLfloat[m_Data.maxVertexPoolSize];
	m_IndexPool = new GLuint[m_Data.maxIndexPoolSize];
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
				(*iter)->OnDraw();
				const auto& render = (*iter)->m_Render;
				if (render.model == currentModel)
				{
					if (m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool) < render.vertexBufferSize
						|| m_Data.maxIndexPoolSize - (indexPos - m_IndexPool) < render.indexCount * sizeof(GLuint))
					{
						FlushAndReset();
					}
					PoolOver(render);
				}
				else
				{
					FlushAndReset();
					currentModel = render.model;
					Renderer::rvaos->end();
					Renderer::rvaos->find(currentModel);
					//if (Renderer::rvaos->find(currentModel) == Renderer::rvaos->end())
					//{
					//	// TODO
					//	// A) create new vertex array and set it to rvaos[currentModel].
					//	// B) parse currentModel layout, bind new vao and generate new buffers for pools (use alloc for pools, but don't send actual data?).
					//	// C) call glVertexAttribPointer on layouts to confirm VAO. at end, unbind vao for security.
					//}
					PoolOver(render);
				}
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
	memcpy_s(vertexPos, m_Data.maxVertexPoolSize - (vertexPos - m_VertexPool), render.vertexBufferData, render.vertexBufferSize);
	memcpy_s(indexPos, m_Data.maxIndexPoolSize - (indexPos - m_IndexPool), render.indexBufferData, render.indexCount * sizeof(GLuint));
	for (PointerOffset ic = 0; ic < render.indexCount * sizeof(GLuint); ic++)
		*(indexPos + ic) += (vertexPos - m_VertexPool) / currentModel.layoutSize();
	vertexPos += render.vertexBufferSize;
	indexPos += render.indexCount * sizeof(GLuint);
}

inline void CanvasLayer::FlushAndReset()
{
	// TODO glBufferSubData(vertexPool) --> glBufferSubData(indexPool) --> bind Renderer::rvaos[currentModel] --> glDrawElements --> unbind vao
	vertexPos = m_VertexPool;
	indexPos = m_IndexPool;
}
