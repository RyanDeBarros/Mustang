#include "render/Renderable.h"

BatchModel::BatchModel(ShaderHandle shader, VertexLayout layout, VertexLayoutMask layoutMask)
	: shader(shader), layout(layout), layoutMask(layoutMask)
{
}

bool BatchModel::operator==(const BatchModel& m) const
{
	return shader == m.shader && layout == m.layout && layoutMask == m.layoutMask;
}

PointerOffset BatchModel::layoutSize() const
{
	// TODO calculate layout size
	return 36;
}

size_t std::hash<BatchModel>::operator()(const BatchModel& model) const
{
	size_t h1 = std::hash<ShaderHandle>()(model.shader);
	size_t h2 = std::hash<ShaderHandle>()(model.layout);
	size_t h3 = std::hash<ShaderHandle>()(model.layoutMask);
	return h1 ^ (h2 << 1) ^ (h3 << 2);
}

namespace Render
{
	Renderable Empty = { BatchModel(), nullptr, 0, 0, nullptr, 0, 0 };
}
