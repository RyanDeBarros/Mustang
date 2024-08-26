#pragma once

#include "AnimationPlayer.h"

template<typename T>
struct KF_Assign : public KeyFrame
{
	T* property;
	T value;
	inline KF_Assign(float timepoint, T* property, const T& value) : KeyFrame(timepoint), property(property), value(value) {}
	inline KF_Assign(float timepoint, T* property, T&& value) : KeyFrame(timepoint), property(property), value(std::move(value)) {}
	inline virtual void operator()() override { *property = value; }
};

template<typename T>
struct KF_AssignSafe : public KeyFrame
{
	T* property;
	T value;
	inline KF_AssignSafe(float timepoint, T* property, const T& value) : KeyFrame(timepoint), property(property), value(value) {}
	inline KF_AssignSafe(float timepoint, T* property, T&& value) : KeyFrame(timepoint), property(property), value(std::move(value)) {}
	inline virtual void operator()() override { if (property) *property = value; }
};

template<typename T>
struct KF_Callback : public KF_Assign<T>
{
	FunctorPtr<void, void> callback;
	inline KF_Callback(float timepoint, T* property, const T& value, const FunctorPtr<void, void>& callback) : KF_Assign<T>(timepoint, property, value), callback(callback) {}
	inline KF_Callback(float timepoint, T* property, const T& value, FunctorPtr<void, void>&& callback) : KF_Assign<T>(timepoint, property, value), callback(std::move(callback)) {}
	inline KF_Callback(float timepoint, T* property, T&& value, const FunctorPtr<void, void>& callback) : KF_Assign<T>(timepoint, property, std::move(value)), callback(callback) {}
	inline KF_Callback(float timepoint, T* property, T&& value, FunctorPtr<void, void>&& callback) : KF_Assign<T>(timepoint, property, std::move(value)), callback(std::move(callback)) {}
	inline virtual void operator()() override { KF_Assign<T>::operator()(); callback(); }
};

template<typename T>
struct KF_CallbackSafe : public KF_AssignSafe<T>
{
	FunctorPtr<void, void> callback;
	inline KF_CallbackSafe(float timepoint, T* property, const T& value, const FunctorPtr<void, void>& callback) : KF_AssignSafe<T>(timepoint, property, value), callback(callback) {}
	inline KF_CallbackSafe(float timepoint, T* property, const T& value, FunctorPtr<void, void>&& callback) : KF_AssignSafe<T>(timepoint, property, value), callback(std::move(callback)) {}
	inline KF_CallbackSafe(float timepoint, T* property, T&& value, const FunctorPtr<void, void>& callback) : KF_AssignSafe<T>(timepoint, property, std::move(value)), callback(callback) {}
	inline KF_CallbackSafe(float timepoint, T* property, T&& value, FunctorPtr<void, void>&& callback) : KF_AssignSafe<T>(timepoint, property, std::move(value)), callback(std::move(callback)) {}
	inline virtual void operator()() override { KF_AssignSafe<T>::operator()(); callback(); }
};

struct KF_Event : public KeyFrame
{
	FunctorPtr<void, void> event;
	inline KF_Event(float timepoint, const FunctorPtr<void, void>& event) : KeyFrame(timepoint), event(event) {}
	inline KF_Event(float timepoint, FunctorPtr<void, void>&& event) : KeyFrame(timepoint), event(std::move(event)) {}
	inline virtual void operator()() override { event(); }
};
