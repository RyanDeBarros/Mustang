#pragma once

#include <GL/glew.h>
#include <map>
#include <list>
#include <variant>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "EngineSettings.h"
#include "Typedefs.h"
#include "Transform.h"
#include "ActorPrimitive.h"
#include "ActorComposite.h"

struct CanvasLayerData
{
	ZIndex z;
	bool enableGLBlend;
	GLenum sourceBlend, destBlend;
	float pLeft, pRight, pBottom, pTop;
	CanvasLayerData(ZIndex z)
		: z(z), enableGLBlend(true), sourceBlend(GL_SRC_ALPHA), destBlend(GL_ONE_MINUS_SRC_ALPHA), pLeft(0), pRight(EngineSettings::window_width), pBottom(0), pTop(EngineSettings::window_height)
	{}
};

class CanvasLayer
{
	CanvasLayerData m_Data;
	glm::mat3 m_Proj;
	Transform2D m_CameraTransform;
	glm::mat3 m_View;
	std::map<ZIndex, std::list<std::variant<ActorPrimitive2D*, ActorComposite2D*>>>* m_Batcher;
public:
	CanvasLayer(ZIndex z = 0);
	CanvasLayer(CanvasLayerData data);
	~CanvasLayer();

	void OnAttach(const ActorPrimitive2D* const primitive);
	void OnAttach(const ActorComposite2D* const composite);
	void OnSetZIndex(const ActorPrimitive2D* const primitive, const ZIndex new_val);
	void OnSetZIndex(const ActorComposite2D* const composite, const ZIndex new_val);
	void OnDetach(const ActorPrimitive2D* const primitive);
	void OnDetach(const ActorComposite2D* const composite);
	void OnDraw();
	
	ZIndex getZIndex() const { return m_Data.z; }
	CanvasLayerData& getDataRef() { return m_Data; }
};
