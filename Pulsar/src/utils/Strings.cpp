#include "Strings.h"

#include <sstream>

#include "render/transform/Transform.h"

std::string STR(const glm::vec2& vec2)
{
	return Concat("", "<", vec2[0], ", ", vec2[1], ">");
}

std::string STR(const glm::vec3& vec3)
{
	return Concat("", "<", vec3[0], ", ", vec3[1], ", ", vec3[2], ">");
}

std::string STR(const glm::vec4& vec4)
{
	return Concat("", "<", vec4[0], ", ", vec4[1], ", ", vec4[2], ", ", vec4[3], ">");
}

std::string STR(const Transform2D& tr)
{
	return Concat("", "[P", STR(tr.position), " R<", tr.rotation, "> S", STR(tr.scale), "]");
}

std::string file_extension_of(const std::string& filepath)
{
	return filepath.substr(filepath.find_last_of('.') + 1);
}
