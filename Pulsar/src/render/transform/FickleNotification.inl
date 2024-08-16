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
	virtual void Notify(FickleSyncCode) {}
};
