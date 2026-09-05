#pragma once

#include "ioINIThread.h"
#include "iocpQueue.h"
#include "ioININode.h"
#include "../include/MemPooler.h"

class ioINIWriter  : public ioINIThread, iocpQueue
{
public:
	ioINIWriter(void);
	virtual ~ioINIWriter(void);

public:
	void Init();
	void Destroy();

public:
	void Run();

public:
	void SaveData( ioININode* node );
	void PushNode( ioININode* node );

public:
	bool GetRunState() const { return m_runState; }
	void SetRunState(bool val) { m_runState = val; }
	ioININode* GetDataNode();

protected:
	bool m_runState;
	MemPooler<ioININode> m_ININodePool;
};