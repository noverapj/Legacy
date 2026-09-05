#include "../stdafx.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../WemadeLog/ioWemadeLogger.h"
#include "NodeInfoManager.h"
#include "../Network/SP2Packet.h"

extern ioWemadeLogger g_WemadeLogger;
extern bool tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens);

NodeInfoManager::NodeInfoManager(void)
{
}


NodeInfoManager::~NodeInfoManager(void)
{
}

void NodeInfoManager::Clear()
{
	for(int i = 0 ; i <= NODE_TYPE_WEMADE_LOG; i++)
	{
		m_Nodes[i].clear();
	}
}

bool NodeInfoManager::Load()
{
	Information( "Global Define..." );

	Clear();

	ioINILoader kLoader( "../global_define.ini" );

	TCHAR szKey[64], szValue[200];
	const int iMaxCount = 512;

	// Billing
	kLoader.SetTitle("Billing");
	for(int i = 1 ; i <= iMaxCount ; i++)
	{
		sprintf_s( szKey, sizeof(szKey), "%d", i );
		kLoader.LoadString(szKey, "", szValue, sizeof(szValue));
		if(!strcmp(szValue, "")) break;

		if(MakePacket(NODE_TYPE_BILLING, "Billing", szValue) == false)	
			return false;
	}

	// DBAgent LOG
	kLoader.SetTitle("DBA_Log");
	for(int i = 1 ; i <= iMaxCount ; i++)
	{
		sprintf_s( szKey, sizeof(szKey), "%d", i );
		kLoader.LoadString(szKey, "", szValue, sizeof(szValue));
		if(!strcmp(szValue, "")) break;

		if(MakePacket(NODE_TYPE_DBAGENT_LOG, "DBA_Log", szValue) == false)	
			return false;
	}

	// DBAgent Game
	kLoader.SetTitle("DBA_Game");
	for(int i = 1 ; i <= iMaxCount ; i++)
	{
		sprintf_s( szKey, sizeof(szKey), "%d", i );
		kLoader.LoadString(szKey, "", szValue, sizeof(szValue));
		if(!strcmp(szValue, "")) break;

		if(MakePacket(NODE_TYPE_DBAGENT_GAME, "DBA_Game", szValue) == false)	
			return false;
	}

	// Log Server
	kLoader.SetTitle("Log");
	for(int i = 1 ; i <= iMaxCount ; i++)
	{
		sprintf_s( szKey, sizeof(szKey), "%d", i );
		kLoader.LoadString(szKey, "", szValue, sizeof(szValue));
		if(!strcmp(szValue, "")) break;

		if(MakePacket(NODE_TYPE_LOGSERVER, "Log", szValue) == false)	
			return false;
	}

	// Wemade Log Server
	kLoader.SetTitle("Wemade_Log");
	for(int i = 1 ; i <= iMaxCount ; i++)
	{
		sprintf_s( szKey, sizeof(szKey), "%d", i );
		kLoader.LoadString(szKey, "", szValue, sizeof(szValue));
		if(!strcmp(szValue, "")) break;

		if(MakePacket(NODE_TYPE_WEMADE_LOG, "Wemade_Log", szValue) == false)	
			return false;
	}

	Information( "done\n" );
	return true;
}

bool NodeInfoManager::ConnectTo()
{
	Information( "Game DBAgent ConnectTo..." );
	if( !g_DBClient.ConnectTo() )	return false;
	Information( "done\n" );

	Information( "Log DBAgent ConnectTo..." );
	if( !g_LogDBClient.ConnectTo() ) return false;
	Information( "done\n" );

	Information( "Wemade Log ConnectTo..." );
	if( !g_WemadeLogger.Create() ) return false;
	Information( "done\n" );

	return true;
}

bool NodeInfoManager::Verify()
{
	TCHAR szValue[200];
	ioINILoader kLoader( "../global_define.ini" );
	kLoader.SetTitle("Billing");
	kLoader.LoadString("1", "0", szValue, sizeof(szValue));
	if(szValue[0] == '0')
	{
		LOG.PrintTimeAndLog(0,"[Billing Info] Load Error");
		return false;
	}
	return true;
}

bool NodeInfoManager::MakeInfoPacket(SP2Packet& kPacket)
{ 
	TCHAR szIP[64];
	for(int i = 0 ; i <= NODE_TYPE_WEMADE_LOG; i++)
	{
		// size 입력
		kPacket << (int)m_Nodes[i].size();

		// 노드정보 입력
		for(NODES::iterator it = m_Nodes[i].begin() ; it != m_Nodes[i].end() ; ++it)
		{
			TOKENS tokens = *it;

			if(tokens.size() == 1)
			{
				strcpy_s(szIP, tokens[0].c_str());
				kPacket << szIP;
			}
			else if(tokens.size() == 2)
			{
				strcpy_s(szIP, tokens[0].c_str());
				kPacket << szIP;
				kPacket << atoi(tokens[1].c_str());
			}
		}
	}
	return true;
}


bool NodeInfoManager::MakePacket( int iNodeType, TCHAR* szTitle, TCHAR* szValue )
{
	if(strlen(szValue) == 0) return false;

	std::string values = szValue;
	std::string delimiter = ":";
	std::vector<std::string> tokens;
	tokenize(values, delimiter, tokens);
	if(tokens.size() == 1)
	{
		m_Nodes[iNodeType].push_back( tokens );
		LOG.PrintTimeAndLog(0,"Global Define [%s] %s", szTitle, tokens[0].c_str());
	}
	else if(tokens.size() == 2)
	{
		m_Nodes[iNodeType].push_back( tokens );
		LOG.PrintTimeAndLog(0,"Global Define [%s] %s:%d", szTitle, tokens[0].c_str(), atoi(tokens[1].c_str()));
	}
	else
	{
		LOG.PrintTimeAndLog(0,"Global Define [%s] Wrong Data", szTitle);
	}

	if(NODE_TYPE_WEMADE_LOG == iNodeType)
	{
		g_WemadeLogger.Register( tokens[0].c_str() );
	}

	return true;
}
