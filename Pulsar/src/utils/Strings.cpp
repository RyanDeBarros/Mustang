#include "Strings.h"

#include <sstream>

#include "render/transform/Transform.h"

std::string Concat(const std::string& delim)
{
	return "";
}

template<typename T>
std::string Concat(const std::string& delta, const T& last)
{
	std::stringstream ss;
	ss << last;
	return ss.str();
}

template<typename T, typename... Args>
std::string Concat(const std::string& delim, const T& first, const Args&... args)
{
	std::stringstream ss;
	ss << first << delim;
	return ss.str() + Concat(delim, args...);
}

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
