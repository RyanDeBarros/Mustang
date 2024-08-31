#pragma once

enum class FickleSyncCode
{
	SyncAll,
	SyncT,
	SyncP,
	SyncRS,
	SyncM
};

struct FickleNotification
{
	virtual ~FickleNotification() = default;
	virtual void Notify(FickleSyncCode) {}
};
