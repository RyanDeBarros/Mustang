#include "render/CanvasLayer.h"

#include "Utility.h"
#include "ActorRenderIterator.h"

CanvasLayer::CanvasLayer(CanvasIndex z)
	: m_Data(CanvasLayerData(z)), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<ActorRenderBase2D>*>();
}

CanvasLayer::CanvasLayer(CanvasLayerData data)
	: m_Data(data), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
{
	m_CameraTransform = Transform2D();
	m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
	m_Batcher = new std::map<ZIndex, std::list<ActorRenderBase2D>*>();
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
	if (m_Data.enableGLBlend)
	{
		TRY(glEnable(GL_BLEND));
		TRY(glBlendFunc(m_Data.sourceBlend, m_Data.destBlend));
	}
	else
	{
		TRY(glDisable(GL_BLEND));
	}
	ActorRenderIterator iter;
	BatchModel currentModel;
	for (const auto& list : *m_Batcher)
	{
		for (const auto element : *list.second)
		{
			iter = std::visit([](auto&& arg) { return ActorRenderIterator(arg); }, element);
			while (*iter)
			{
				const auto& transform = (*iter)->m_Transform;
				const auto& render = (*iter)->m_Render;
				if (render.model == currentModel)
				{
					// TODO copy render/transform over to this canvas layer's pooled vertex buffer and index buffer. If limit is reached, flush and reset the pools.
				}
				else
				{
					// TODO 1. FLUSH: i) bind pools and glBufferSubData on those pools. ii) unbind pools and bind Renderer::rvaos[currentModel]. iii) glDrawElements and unbind vao.
					//		2. RESET: i) reset pools and set currentModel = render.model. ii) if rvaos has currentModel, continue. else: create vao by:
					//									A) create new vertex array and set it to rvaos[currentModel].
					//									B) parse currentModel layout, bind new vao and generate new buffers for pools (use alloc for pools, but don't send actual data?).
					//									C) call glVertexAttribPointer on layouts to confirm VAO. at end, unbind vao for security.
					//		3. copy render.transform over to this canvas layer's pooled vertex buffer and index buffer. If limit is reached, flush and reset the pools.
				}
			}
		}
	}
}
