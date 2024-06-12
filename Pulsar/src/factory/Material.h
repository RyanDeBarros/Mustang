#pragma once

#include <vector>

enum MaterialUniformType
{
	Int1,
	Int2,
	Int3,
	Int4,
	UInt1,
	UInt2,
	UInt3,
	UInt4,
	Float1,
	Float2,
	Float3,
	Float4,
	Matrix2,
	Matrix3,
	Matrix4
};

struct MaterialUniform
{
	const char* name;
	MaterialUniformType type;
	void* data;
};

class Material
{
	friend class MaterialFactory;
	std::vector<MaterialUniform> m_Uniforms;
	inline bool equivalent(const std::vector<MaterialUniform>& uniforms) { return m_Uniforms == uniforms; }

public:
	Material(const std::vector<MaterialUniform>& uniform_vector);
	~Material();
};
