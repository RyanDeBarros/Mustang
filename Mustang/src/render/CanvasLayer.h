#pragma once

#include <GL/glew.h>
#include <forward_list>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "EngineSettings.h"
#include "Typedefs.h"
#include "Transform.h"
#include "ActorPrimitive.h"
#include "ActorConstruct.h"

struct CanvasLayerData
{
	ZIndex z;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	float pLeft, pRight, pBottom, pTop;
	bool relativeOrderByInsert;
	CanvasLayerData(ZIndex z)
		: z(z), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA), pLeft(0), pRight(EngineSettings::window_width), pBottom(0), pTop(EngineSettings::window_height), relativeOrderByInsert(true)
	{}
};

class CanvasLayer
{
	CanvasLayerData m_Data;
	glm::mat3 m_Proj;
	Transform2D m_CameraTransform;
	glm::mat3 m_View;
	std::forward_list<ActorPrimitive2D>* m_BatchList;

	CanvasLayer() : m_Data(CanvasLayerData(0)), m_Proj(glm::ortho<float>(m_Data.pLeft, m_Data.pRight, m_Data.pBottom, m_Data.pTop))
	{
		m_CameraTransform = Transform2D();
		m_View = Transform::ToMatrix(Transform::Inverse(m_CameraTransform));
		m_BatchList = new std::forward_list<ActorPrimitive2D>();
	}
public:
	CanvasLayer(ZIndex);
	CanvasLayer(CanvasLayerData data);
	~CanvasLayer();

	CanvasHandle OnAttach(ActorPrimitive2D* primitive);
	CanvasHandle OnAttach(ActorConstruct2D* construct);
	void OnSetZIndex(const CanvasHandle& handle, ZIndex new_val);
	void OnDetach(const CanvasHandle& handle);
	void OnDraw();
	
	ZIndex getZIndex() const { return m_Data.z; }
	CanvasLayerData& getDataRef() { return m_Data; }
};
