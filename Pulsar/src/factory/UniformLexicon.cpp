#include "UniformLexicon.h"

UniformLexicon::UniformLexicon(const std::map<std::string, Uniform>& uniform_set)
	: m_Uniforms(uniform_set)
{
}

UniformLexicon::~UniformLexicon()
{
}

void UniformLexicon::MergeLexicon(const UniformLexicon& lexicon)
{
	m_Uniforms.insert(lexicon.m_Uniforms.begin(), lexicon.m_Uniforms.end());
}

bool UniformLexicon::Shares(const UniformLexicon& lexicon)
{
	auto ait = m_Uniforms.begin();
	auto bit = lexicon.m_Uniforms.begin();
	while (ait != m_Uniforms.end() && bit != lexicon.m_Uniforms.end())
	{
		const std::string akey = ait->first;
		const std::string bkey = bit->first;
		if (akey < bkey)
			ait++;
		else if (akey > bkey)
			bit++;
		else if (ait->second != bit->second)
				return false;
	}
	return true;
}

const Uniform* UniformLexicon::GetValue(const std::string& name)
{
	auto it = m_Uniforms.find(name);
	if (it != m_Uniforms.end())
		return &it->second;
	else
		return nullptr;
}

bool UniformLexicon::SetValue(const std::string& name, const Uniform& uniform) {
	auto it = m_Uniforms.find(name);
	if (it != m_Uniforms.end())
	{
		if (it->second != uniform)
		{
			it->second = uniform;
			return true;
		}
	}
	return false;
}

bool UniformLexicon::DefineNewValue(const std::string& name, const Uniform& uniform)
{
	auto it = m_Uniforms.find(name);
	if (it == m_Uniforms.end())
	{
		m_Uniforms.insert({ name, uniform });
		return true;
	}
	return false;
}