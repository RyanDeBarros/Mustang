#pragma once

#include <glm/glm.hpp>
#include <string>

extern std::string Concat(const std::string& delim);
template<typename T>
extern std::string Concat(const std::string& delta, const T& last);
template<typename T, typename... Args>
extern std::string Concat(const std::string& delim, const T& first, const Args&... args);

extern std::string STR(const glm::vec2& vec2);
extern std::string STR(const glm::vec3& vec3);
extern std::string STR(const glm::vec4& vec4);
extern std::string STR(const struct Transform2D& tr);
