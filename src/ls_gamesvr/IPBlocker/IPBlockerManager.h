#pragma once

#include "../IPBlocker/ioIPBlocker.h"

class IPBlockerManager
{
public:
	IPBlockerManager();
	virtual ~IPBlockerManager();

	void Init();
	void Destroy();
	void Load();

public:
	BOOL CheckBlackList(char* IP);
	BOOL CheckWhiteList(char* IP);

public:
	void SetWhiteList(BOOL bWhiteListOn)	{ m_bWhiteListOn = bWhiteListOn; }
	BOOL IsWhiteListOn()	{ return m_bWhiteListOn; }

private:
	ioIPBlocker m_vBlackList;
	ioIPBlocker m_vWhiteList;
	BOOL		m_bWhiteListOn;
};