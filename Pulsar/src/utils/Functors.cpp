#include "Functors.h"

std::function<float(float)> ConstantFunc(float c)
{
	return [=](float t) -> float { return c; };
}

std::function<float(float)> StepFunc(float at, float height, bool dirLR, bool incl, float lower)
{
	if (dirLR)
		return [=](float t) -> float { return (t < at) || (!incl && t == at) ? lower : height; };
	else
		return [=](float t) -> float { return (t > at) || (!incl && t == at) ? lower : height; };
}

std::function<float(float)> LinearFunc(float slope, float intercept)
{
	return [=](float t) -> float { return slope * t + intercept; };
}

std::function<float(float)> QuadraticFunc(float a, float x_intercept, float y_intercept)
{
	return [=](float t) -> float { return a * static_cast<float>(glm::pow(t - x_intercept, 2)) + y_intercept; };
}

std::function<float(float)> PowerFunc(float a, float exp)
{
	return [=](float t) -> float { return a * static_cast<float>(glm::pow(t, exp)); };
}

std::function<float(float)> FuncSum(const std::function<float(float)>& f1, const std::function<float(float)>& f2)
{
	return [&](float t) -> float { return f1(t) + f2(t); };
}

std::function<float(float)> FuncMul(const std::function<float(float)>& f1, const std::function<float(float)>& f2)
{
	return [&](float t) -> float { return f1(t) * f2(t); };
}

std::function<glm::vec4(float)> LinearCombo4x1(const glm::vec4& constant, const glm::vec4& coeff1)
{
	return [&](float t) -> glm::vec4 { return constant + coeff1 * t; };
}

std::function<glm::vec4(const glm::vec2&)> LinearCombo4x2(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2)
{
	return [&](const glm::vec2& v) -> glm::vec4 { return constant + coeff1 * v[0] + coeff2 * v[1]; };
}

std::function<glm::vec4(const glm::vec3&)> LinearCombo4x3(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3)
{
	return [&](const glm::vec3& v) -> glm::vec4 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2]; };
}

std::function<glm::vec4(const glm::vec4&)> LinearCombo4x4(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3, const glm::vec4& coeff4)
{
	return [&](const glm::vec4& v) -> glm::vec4 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2] + coeff4 * v[3]; };
}

std::function<glm::vec3(float)> LinearCombo3x1(const glm::vec3& constant, const glm::vec3& coeff1)
{
	return [&](float t) -> glm::vec3 { return constant + coeff1 * t; };
}

std::function<glm::vec3(const glm::vec2&)> LinearCombo3x2(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2)
{
	return [&](const glm::vec2& v) -> glm::vec3 { return constant + coeff1 * v[0] + coeff2 * v[1]; };
}

std::function<glm::vec3(const glm::vec3&)> LinearCombo3x3(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3)
{
	return [&](const glm::vec3& v) -> glm::vec3 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2]; };
}

std::function<glm::vec3(const glm::vec4&)> LinearCombo3x4(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3, const glm::vec3& coeff4)
{
	return [&](const glm::vec4& v) -> glm::vec3 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2] + coeff4 * v[3]; };
}

std::function<glm::vec2(float)> LinearCombo2x1(const glm::vec2& constant, const glm::vec2& coeff1)
{
	return [&](float t) -> glm::vec2 { return constant + coeff1 * t; };
}

std::function<glm::vec2(const glm::vec2&)> LinearCombo2x2(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2)
{
	return [&](const glm::vec2& v) -> glm::vec2 { return constant + coeff1 * v[0] + coeff2 * v[1]; };
}

std::function<glm::vec2(const glm::vec3&)> LinearCombo2x3(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3)
{
	return [&](const glm::vec3& v) -> glm::vec2 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2]; };
}

std::function<glm::vec2(const glm::vec4&)> LinearCombo2x4(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3, const glm::vec2& coeff4)
{
	return [&](const glm::vec4& v) -> glm::vec2 { return constant + coeff1 * v[0] + coeff2 * v[1] + coeff3 * v[2] + coeff4 * v[3]; };
}


// TODO typedef these operator functions
std::function<void(float&)> OperateFloatMult(float mult)
{
	return [mult](float& t) -> void { t *= mult; };
}

std::function<std::function<void(float&)>(float)> OperateFloatMultWrap = [](float t) -> std::function<void(float&)> { return OperateFloatMult(t); };
std::function<unsigned int(float)> CastFloatToUInt = [](float t) -> unsigned int { return static_cast<unsigned int>(t); };
std::function<float(float)> Identity = [](float t) -> float { return t; };
