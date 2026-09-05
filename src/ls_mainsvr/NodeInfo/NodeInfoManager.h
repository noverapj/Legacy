#pragma once

#include "../../include/cSingleton.h"

class SP2Packet;

enum NodeTypes
{
	NODE_TYPE_BILLING = 0,
	NODE_TYPE_DBAGENT_LOG,
	NODE_TYPE_DBAGENT_GAME,
	NODE_TYPE_LOGSERVER,
	NODE_TYPE_WEMADE_LOG,
	NODE_TYPE_END
};

class NodeInfoManager
{
public:
	NodeInfoManager(void);
	virtual ~NodeInfoManager(void);

protected:
	void Clear();

public:
	bool Load();
	bool ConnectTo();
	bool Verify();
	bool MakeInfoPacket( SP2Packet& packet );
	bool MakePacket( int iNodeType, TCHAR* szTitle, TCHAR* szValue );

protected:
	typedef std::vector<std::string> TOKENS;
	typedef std::vector<TOKENS> NODES;

	NODES m_Nodes[NODE_TYPE_END];
};

typedef cSingleton<NodeInfoManager> S_NodeInfoManager;
#define g_NodeInfoManager (*S_NodeInfoManager::GetInstance())

 
	