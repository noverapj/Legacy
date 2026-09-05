#include "stdafx.h"
#include "iochannelingnodemgame.h"
#include ".\iochannelingnodemanager.h"
#include "iochannelingnodedaum.h"
#include "ioChannelingNodeNaver.h"
#include "ioChannelingNodeTooniland.h"
#include "ioChannelingNodeWemadeBuy.h"
#include "ioChannelingNodeSteam.h"
#include "ioChannelingNodeNexonBuy.h"
#include "ioChannelingNodeHangame.h"
#include "ioChannelingNodeValofe.h"
#ifdef  __OHTG_BILLING_TAIWAN_HAPPYTUK__
#include "ioChannelingNodeHappyTuk.h"
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__


template<> ioChannelingNodeManager* Singleton< ioChannelingNodeManager >::ms_Singleton = 0;

ioChannelingNodeManager::ioChannelingNodeManager(void)
{
	m_vChannelingNodeVector.reserve( 10 );
}

ioChannelingNodeManager::~ioChannelingNodeManager(void)
{
	for(vChannelingNodeVector::iterator iter = m_vChannelingNodeVector.begin(); iter != m_vChannelingNodeVector.end(); ++iter)
	{
	    delete *iter;
	}
	m_vChannelingNodeVector.clear();
}

ioChannelingNodeManager & ioChannelingNodeManager::GetSingleton()
{
	return Singleton< ioChannelingNodeManager >::GetSingleton();
}

void ioChannelingNodeManager::Init()
{
	AddNode( CreateNode( CNT_WEMADEBUY ) );
	AddNode( CreateNode( CNT_MGAME ) );
	AddNode( CreateNode( CNT_DAUM ) );
	AddNode( CreateNode( CNT_NAVER ) );
	AddNode( CreateNode( CNT_TOONILAND ) );
	AddNode( CreateNode( CNT_NEXON ) );
	AddNode( CreateNode( CNT_STEAM ) );
	AddNode( CreateNode( CNT_HANGAME ) );
	AddNode( CreateNode( CNT_VALOFE ) );
#ifdef  __OHTG_BILLING_TAIWAN_HAPPYTUK__
	AddNode( CreateNode( CNT_HAPPYTUK ) );
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__


	// 새로운 채널링 추가
}

ioChannelingNodeParent * ioChannelingNodeManager::CreateNode( ChannelingType eChannelingType )
{
	ioChannelingNodeParent *pNode = NULL;
	if( eChannelingType == CNT_WEMADEBUY )
		pNode = new ioChannelingNodeWemadeBuy;
	else if( eChannelingType == CNT_MGAME )
		pNode = new ioChannelingNodeMgame;
	else if( eChannelingType == CNT_DAUM )
		pNode = new ioChannelingNodeDaum;
	else if( eChannelingType == CNT_NAVER )
		pNode = new ioChannelingNodeNaver;
	else if( eChannelingType == CNT_TOONILAND )
		pNode = new ioChannelingNodeTooniland;
	else if (eChannelingType == CNT_NEXON )
		pNode = new ioChannelingNodeNexonBuy;
	else if( eChannelingType == CNT_HANGAME )
		pNode = new ioChannelingNodeHangame;
	else if (eChannelingType == CNT_STEAM )
		pNode = new ioChannelingNodeSteam;
	else if( eChannelingType == CNT_VALOFE )
		pNode = new ioChannelingNodeValofe;
#ifdef  __OHTG_BILLING_TAIWAN_HAPPYTUK__
	else if( eChannelingType == CNT_HAPPYTUK )
		pNode = new ioChannelingNodeHappyTuk;
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__
	// 새로운 채널링 추가
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Type is wrong. %d", __FUNCTION__, (int) eChannelingType );

	return pNode;
}

void ioChannelingNodeManager::AddNode( ioChannelingNodeParent *pNode )
{
	if( !pNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL." , __FUNCTION__ );
		return;
	}

	if( GetNode( pNode->GetType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Type is existing.%d" , __FUNCTION__ , pNode->GetType() );
		return;
	}

	m_vChannelingNodeVector.push_back( pNode );
}

ioChannelingNodeParent * ioChannelingNodeManager::GetNode( ChannelingType eChannelingType )
{
	for(vChannelingNodeVector::iterator iter = m_vChannelingNodeVector.begin(); iter != m_vChannelingNodeVector.end(); ++iter)
	{
	    ioChannelingNodeParent *pNode = *iter;
		if( !pNode )
			continue;
		if( pNode->GetType() == eChannelingType )
			return pNode;
	}

	return NULL;
}