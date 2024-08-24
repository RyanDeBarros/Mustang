#pragma once

#include <glm/glm.hpp>
#include <string>

inline std::string Concat(const std::string& delim)
{
	return "";
}
template<typename T>
inline std::string Concat(const std::string& delta, const T& last)
{
	std::stringstream ss;
	ss << last;
	return ss.str();
}
template<typename T, typename... Args>
inline std::string Concat(const std::string& delim, const T& first, const Args&... args)
{
	std::stringstream ss;
	ss << first << delim;
	return ss.str() + Concat(delim, args...);
}

extern std::string STR(const glm::vec2& vec2);
extern std::string STR(const glm::vec3& vec3);
extern std::string STR(const glm::vec4& vec4);
extern std::string STR(const struct Transform2D& tr);

extern std::string file_extension_of(const std::string& filepath);
