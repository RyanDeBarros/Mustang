#pragma once

#include <unordered_map>
#include <stdexcept>

struct RegistryFullException : std::runtime_error
{
	RegistryFullException(const char* message = "Registry ran out of handle space.") : std::runtime_error(message) {}
	RegistryFullException(const std::string& message) : std::runtime_error(message) {}
};

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2 = void, typename ConstructArgs_3 = void>
class Registry
{
protected:
	Handle current_handle = 1;
	std::unordered_map<Handle, Element> registry;
	std::unordered_map<ConstructArgs_1, Handle> lookup_1;
	std::unordered_map<ConstructArgs_2, Handle> lookup_2;
	std::unordered_map<ConstructArgs_3, Handle> lookup_3;

public:
	static constexpr Handle HANDLE_CAP = Handle(-1);

	Registry() = default;
	Registry(const Registry&) = delete;
	Registry(Registry&&) = delete;
	virtual ~Registry() = default;

	Element const* Get(Handle handle) const;
	Element* Get(Handle handle);
	bool Destroy(Handle handle);
	Handle GetHandle(const ConstructArgs_1& args);
	Handle GetHandle(const ConstructArgs_2& args);
	Handle GetHandle(const ConstructArgs_3& args);
	Element const* Emplace(const ConstructArgs_1& args) const;
	Element const* Emplace(const ConstructArgs_2& args) const;
	Element const* Emplace(const ConstructArgs_3& args) const;
	Element* Emplace(const ConstructArgs_1& args);
	Element* Emplace(const ConstructArgs_2& args);
	Element* Emplace(const ConstructArgs_3& args);
	Handle Register(Element&& element);
};

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
class Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, void>
{
protected:
	Handle current_handle = 1;
	std::unordered_map<Handle, Element> registry;
	std::unordered_map<ConstructArgs_1, Handle> lookup_1;
	std::unordered_map<ConstructArgs_2, Handle> lookup_2;

public:
	static constexpr Handle HANDLE_CAP = Handle(-1);

	Registry() = default;
	Registry(const Registry&) = delete;
	Registry(Registry&&) = delete;
	virtual ~Registry() = default;

	Element const* Get(Handle handle) const;
	Element* Get(Handle handle);
	bool Destroy(Handle handle);
	Handle GetHandle(const ConstructArgs_1& args);
	Handle GetHandle(const ConstructArgs_2& args);
	Element const* Emplace(const ConstructArgs_1& args) const;
	Element const* Emplace(const ConstructArgs_2& args) const;
	Element* Emplace(const ConstructArgs_1& args);
	Element* Emplace(const ConstructArgs_2& args);
	Handle Register(Element&& element);
};

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
class Registry<Element, Handle, ConstructArgs_1, void, void>
{
protected:
	Handle current_handle = 1;
	std::unordered_map<Handle, Element> registry;
	std::unordered_map<ConstructArgs_1, Handle> lookup_1;

public:
	static constexpr Handle HANDLE_CAP = Handle(-1);

	Registry() = default;
	Registry(const Registry&) = delete;
	Registry(Registry&&) = delete;
	virtual ~Registry() = default;

	Element const* Get(Handle handle) const;
	Element* Get(Handle handle);
	bool Destroy(Handle handle);
	Handle GetHandle(const ConstructArgs_1& args);
	Element const* Emplace(const ConstructArgs_1& args) const;
	Element* Emplace(const ConstructArgs_1& args);
	Handle Register(Element&& element);
};

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Get(Handle handle) const
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Get(Handle handle)
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
bool Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Destroy(Handle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::GetHandle(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::GetHandle(const ConstructArgs_2& args)
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::GetHandle(const ConstructArgs_3& args)
{
	auto iter = lookup_3.find(args);
	if (iter != lookup_3.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_3[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_1& args) const
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_2& args) const
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_2& args)
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_3& args) const
{
	auto iter = lookup_3.find(args);
	if (iter != lookup_3.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_3[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Emplace(const ConstructArgs_3& args)
{
	auto iter = lookup_3.find(args);
	if (iter != lookup_3.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_3[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2, typename ConstructArgs_3>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2, ConstructArgs_3>::Register(Element&& element)
{
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Get(Handle handle) const
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Get(Handle handle)
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
bool Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Destroy(Handle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::GetHandle(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::GetHandle(const ConstructArgs_2& args)
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Emplace(const ConstructArgs_1& args) const
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}


template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Emplace(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element const* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Emplace(const ConstructArgs_2& args) const
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Element* Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Emplace(const ConstructArgs_2& args)
{
	auto iter = lookup_2.find(args);
	if (iter != lookup_2.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_2[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1, typename ConstructArgs_2>
Handle Registry<Element, Handle, ConstructArgs_1, ConstructArgs_2>::Register(Element&& element)
{
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Element const* Registry<Element, Handle, ConstructArgs_1>::Get(Handle handle) const
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Element* Registry<Element, Handle, ConstructArgs_1>::Get(Handle handle)
{
	auto iter = registry.find(handle);
	return iter != registry.end() ? &iter->second : nullptr;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
bool Registry<Element, Handle, ConstructArgs_1>::Destroy(Handle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Handle Registry<Element, Handle, ConstructArgs_1>::GetHandle(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return iter->second;
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return handle;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Element const* Registry<Element, Handle, ConstructArgs_1>::Emplace(const ConstructArgs_1& args) const
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Element* Registry<Element, Handle, ConstructArgs_1>::Emplace(const ConstructArgs_1& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return Get(iter->second);
	Element element(args);
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		auto res = registry.emplace(handle, std::move(element));
		lookup_1[args] = handle;
		return &res.first->second;
	}
	return 0;
}

template<typename Element, std::unsigned_integral Handle, typename ConstructArgs_1>
Handle Registry<Element, Handle, ConstructArgs_1>::Register(Element&& element)
{
	if (element)
	{
		if (current_handle == HANDLE_CAP)
			throw RegistryFullException();
		Handle handle = current_handle++;
		registry.emplace(handle, std::move(element));
		return handle;
	}
	return 0;
}
