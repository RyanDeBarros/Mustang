#pragma once

#include <glm/glm.hpp>
#include <functional>

extern std::function<float(float)> ConstantFunc(float c);
extern std::function<float(float)> StepFunc(float at, float height = 1.0f, bool dirLR = true, bool incl = true, float lower = 0.0f);
extern std::function<float(float)> LinearFunc(float slope = 0, float intercept = 0);
extern std::function<float(float)> QuadraticFunc(float a, float x_intercept, float y_intercept);
extern std::function<float(float)> PowerFunc(float a, float exp);
extern std::function<float(float)> FuncSum(const std::function<float(float)>& f1, const std::function<float(float)>& f2);
extern std::function<float(float)> FuncMul(const std::function<float(float)>& f1, const std::function<float(float)>& f2);

extern std::function<glm::vec4(float)> LinearCombo4x1(const glm::vec4& constant, const glm::vec4& coeff1);
extern std::function<glm::vec4(const glm::vec2&)> LinearCombo4x2(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2);
extern std::function<glm::vec4(const glm::vec3&)> LinearCombo4x3(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3);
extern std::function<glm::vec4(const glm::vec4&)> LinearCombo4x4(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3, const glm::vec4& coeff4);
extern std::function<glm::vec3(float)> LinearCombo3x1(const glm::vec3& constant, const glm::vec3& coeff1);
extern std::function<glm::vec3(const glm::vec2&)> LinearCombo3x2(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2);
extern std::function<glm::vec3(const glm::vec3&)> LinearCombo3x3(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3);
extern std::function<glm::vec3(const glm::vec4&)> LinearCombo3x4(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3, const glm::vec3& coeff4);
extern std::function<glm::vec2(float)> LinearCombo2x1(const glm::vec2& constant, const glm::vec2& coeff1);
extern std::function<glm::vec2(const glm::vec2&)> LinearCombo2x2(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2);
extern std::function<glm::vec2(const glm::vec3&)> LinearCombo2x3(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3);
extern std::function<glm::vec2(const glm::vec4&)> LinearCombo2x4(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3, const glm::vec2& coeff4);

extern std::function<void(float&)> OperateFloatMult(float mult);

extern std::function<std::function<void(float&)>(float)> OperateFloatMultWrap;
extern std::function<unsigned int(float)> CastFloatToUInt;
extern std::function<float(float)> Identity;

template<typename T, typename U, typename V, typename W>
inline std::function<T(W)> Composition(const std::function<T(U)>& f, const std::function<V(W)>& g)
{
	return [f, g](W w) -> T { return f(static_cast<U>(g(w))); };
}

inline std::function<glm::vec2(const glm::vec2&)> Vec2Wrap(const std::function<float(float)>& f, const std::function<float(float)>& g)
{
	return [f, g](const glm::vec2& v) -> glm::vec2 { return { f(v[0]), f(v[1]) }; };
}

template<typename T, typename U, typename V, typename W>
inline std::function<glm::vec2(const glm::vec2&)> Vec2Wrap(const std::function<T(U)>& f, const std::function<V(W)>& g)
{
	return [f, g](const glm::vec2& v) -> glm::vec2 { return { static_cast<float>(f(v[0])), static_cast<float>(f(v[1])) }; };
}
