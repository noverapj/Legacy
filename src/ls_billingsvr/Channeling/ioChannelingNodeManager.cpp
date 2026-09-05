#include "../stdafx.h"
#include "./iochannelingnodemanager.h"


template<> ioChannelingNodeManager* Singleton< ioChannelingNodeManager >::ms_Singleton = 0;
extern CLog LOG;

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
	AddNode( CreateNode( CNT_WEMADE ) );
	AddNode( CreateNode( CNT_TOONILAND ) );
	AddNode( CreateNode( CNT_NEXONSESSION ) );
	AddNode( CreateNode( CNT_NEXON ) );
	AddNode( CreateNode( CNT_HANGAME ) );
	// 새로운 채널링 추가
}

ioChannelingNodeParent * ioChannelingNodeManager::CreateNode( ChannelingType eChannelingType )
{
	ioChannelingNodeParent *pNode = NULL;
	if( eChannelingType == CNT_WEMADEBUY )
		pNode = new ioChannelingNodeWemadeCashLink;
	else if( eChannelingType == CNT_MGAME )
		pNode = new ioChannelingNodeMgame;
	else if( eChannelingType == CNT_DAUM )
		pNode = new ioChannelingNodeDaum_v2;
	else if( eChannelingType == CNT_NAVER )
		pNode = new ioChannelingNodeNaver;
	else if( eChannelingType == CNT_TOONILAND )
		pNode = new ioChannelingNodeTooniland;
	else if( eChannelingType == CNT_NEXON )
		pNode = new ioChannelingNodeNexonBuy_v2;
	else if( eChannelingType == CNT_HANGAME )
		pNode = new ioChannelingNodeHangame;
	else
		LOG.PrintTimeAndLog( 0, "%s Type is wrong. %d", __FUNCTION__, (int) eChannelingType );

	return pNode;
}

void ioChannelingNodeManager::AddNode( ioChannelingNodeParent *pNode )
{
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s pNode == NULL." , __FUNCTION__ );
		return;
	}

	if( GetNode( pNode->GetType() ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Type is existing.%d" , __FUNCTION__ , pNode->GetType() );
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

int ioChannelingNodeManager::GetCount()
{
	int size = 0;
	for(vChannelingNodeVector::iterator iter = m_vChannelingNodeVector.begin(); iter != m_vChannelingNodeVector.end(); ++iter)
	{
		ioChannelingNodeParent *pNode = *iter;
		if( !pNode )
			continue;
		size += pNode->GetCount();
	}
	return size;	
}

