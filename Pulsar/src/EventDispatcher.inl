#pragma once

#include <vector>
#include <unordered_map>

#include "utils/Functor.inl"

template<typename Event>
class EventDispatcher
{
public:
	using Handler = Functor<void, const Event&>;
	using HandlerID = size_t;

	void Emit(const Event& e)
	{
		for (auto iter = handlers.begin(); iter != handlers.end(); ++iter)
			iter->first(e);
	}
	HandlerID Connect(Handler&& handler)
	{
		handlers.push_back({ std::move(handler), nextID });
		return nextID++;
	}
	bool Disconnect(HandlerID id)
	{
		auto iter = std::lower_bound(handlers.begin(), handlers.end(), id, [](const std::pair<Handler, HandlerID>& h1, const std::pair<Handler, HandlerID>& h2) {
			return h1.second < h2.second;
			});
		if (iter != handlers.end() && iter->second == id)
		{
			handlers.erase(iter);
			nextID = (handlers.end() - 1)->second + 1;
			return true;
		}
		return false;
	}

private:
	std::vector<std::pair<Handler, HandlerID>> handlers;
	size_t nextID = 0;
};

template<typename SourceEvent, typename DestinationEvent, typename Bucket>
class EventBucketDispatcher
{
public:
	using Separator = Functor<std::pair<Bucket, DestinationEvent>, const SourceEvent&>;
	using HandlerID = EventDispatcher<DestinationEvent>::HandlerID;
	using Handler = EventDispatcher<DestinationEvent>::Handler;

	EventBucketDispatcher(Separator&& separator) : separator(std::move(separator)) {}

	void Emit(const SourceEvent& e)
	{
		auto sep = separator(e);
		auto iter = dispatchers.find(sep.first);
		if (iter != dispatchers.end())
			iter->second.Emit(sep.second);
	}
	HandlerID Connect(const Bucket& bucket, Handler&& handler)
	{
		return dispatchers[bucket].Connect(std::move(handler));
	}
	bool Disconnect(const Bucket& bucket, HandlerID id)
	{
		auto iter = dispatchers.find(bucket);
		if (iter != dispatchers.end())
			return iter->second.Disconnect(id);
		return false;
	}

private:
	Separator separator;
	std::unordered_map<Bucket, EventDispatcher<DestinationEvent>> dispatchers;
};
