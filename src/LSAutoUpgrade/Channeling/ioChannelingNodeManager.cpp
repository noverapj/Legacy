#include "../stdafx.h"
#include "iochannelingnodenormal.h"
#include "iochannelingnodemgame.h"
#include ".\iochannelingnodemanager.h"
#include "iochannelingnodedaum.h"
#include "ioChannelingNodeBuddy.h"
#include "ioChannelingNodeNaver.h"


template<> ioChannelingNodeManager* Singleton< ioChannelingNodeManager >::ms_Singleton = 0;

ioChannelingNodeManager::ioChannelingNodeManager(void)
{
	m_eChannelingType = CNT_NONE;
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
	AddNode( CreateNode( CNT_NORMAL ) );
	AddNode( CreateNode( CNT_MGAME ) );
	AddNode( CreateNode( CNT_DAUM ) );
	AddNode( CreateNode( CNT_BUDDY ) );
	AddNode( CreateNode( CNT_NAVER ) );
	// 새로운 채널링 추가
}

ioChannelingNodeParent * ioChannelingNodeManager::CreateNode( ChannelingType eChannelingType )
{
	ioChannelingNodeParent *pNode = NULL;
	if( eChannelingType == CNT_NORMAL )
		pNode = new ioChannelingNodeNormal;
	else if( eChannelingType == CNT_MGAME )
		pNode = new ioChannelingNodeMgame;
	else if( eChannelingType == CNT_DAUM )
		pNode = new ioChannelingNodeDaum;
	else if( eChannelingType == CNT_BUDDY )
		pNode = new ioChannelingNodeBuddy;
	else if( eChannelingType == CNT_NAVER )
		pNode = new ioChannelingNodeNaver;
	// 새로운 채널링 추가
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