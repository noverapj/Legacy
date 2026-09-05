#include "stdafx.h"

#include "ioEventUserNode.h"

#include ".\ioeventusermanager.h"

extern CLog EventLOG;

EventUserManager::EventUserManager()
{

}

EventUserManager::~EventUserManager()
{
	Clear();
}

void EventUserManager::Init()
{
	Clear();
	int iSize = g_EventMgr.GetSize();
	for (int i = 0; i < iSize ; i++)
	{
		EventUserNode *pNode = CreatEventUserNode( g_EventMgr.GetType( i ), g_EventMgr.GetModeCategory( i ) );
		if( pNode )
		{
			if( !pNode->IsEmptyValue() )
				pNode->SetSize( g_EventMgr.GetMaxDBValue() + pNode->GetAddSize() );
			pNode->Init();
			m_EventUserNodeVec.push_back( pNode );
		}
	}
}

void EventUserManager::Clear()
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		SAFEDELETE( *it );
	}
	m_EventUserNodeVec.clear();
}

int EventUserManager::GetSize() const
{	
	return m_EventUserNodeVec.size();
}

EventType EventUserManager::GetType( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_EventUserNodeVec.size() ) )
		return EVT_NONE;

	return m_EventUserNodeVec[iArray]->GetType();
}

EventUserNode * EventUserManager::CreatEventUserNode( EventType eEventType, ModeCategory eModeCategory )
{
	EventUserNode *pNode = NULL;
	if( eEventType == EVT_NONE )
		pNode = new EventUserNode;
	else if( eEventType == EVT_PROPOSAL )
		pNode = new ProposalEventUserNode;
	else if( eEventType == EVT_COIN )
		pNode = new CoinEventUserNode;
	else if( eEventType == EVT_EXP )
		pNode = new ExpEventUserNode;
	else if( eEventType == EVT_PESO )
		pNode = new PesoEventUserNode;
	else if( eEventType == EVT_PLAYTIME )
		pNode = new PlayTimeEventUserNode;
	else if( eEventType == EVT_CHILDRENDAY )
		pNode = new ChildrenDayEventUserNode;
	else if( eEventType == EVT_PESOBONUS )
		pNode = new PesoBonusEventUserNode;
	else if( eEventType == EVT_BUY_CHAR_NO_LEVEL_LIMIT )
		pNode = new BuyCharNoLevelLimitEventUserNode;
	else if( eEventType == EVT_GRADEUP )
		pNode = new GradeUpEventUserNode;
	else if( eEventType == EVT_PCROOM_BONUS )
		pNode = new PCRoomEventUserNode;
	else if( eEventType == EVT_CHANCE_MORTMAIN_CHAR )
		pNode = new ChanceMortmainCharEventUserNode;
	else if( eEventType == EVT_ONE_DAY_GOLD_ITEM )
		pNode = new OneDayGoldItemEvent;
	else if( eEventType == EVT_DORMANCY_USER )
		pNode = new DormancyUserEvent;
	else if( eEventType == EVT_PLAYTIME_PRESENT )
		pNode = new PlayTimePresentEventUserNode;
	else if( eEventType == EVT_CHRISTMAS )
		pNode = new ChristmasEventUserNode;
	else if( eEventType == EVT_BUY_ITEM  ||
		     eEventType == EVT_BUY_ITEM_2||
			 eEventType == EVT_BUY_ITEM_3  )
		pNode = new BuyItemEventUserNode;
	else if( eEventType == EVT_FISHING )
		pNode = new FishingEventUserNode;
	else if( eEventType == EVT_EXERCISESOLDIER )
		pNode = new ExerciseSoldierEventUserNode;
	else if( eEventType == EVT_CONNECTION_TIME )
		pNode = new ConnectionTimeEventUserNode;
	else if( eEventType == EVT_ONE_DAY_GIFT  || 
		     eEventType == EVT_ONE_DAY_GIFT_2  )
		pNode = new OneDayGiftEventUserNode;
	else if( eEventType == EVT_GRADEUP_PRESENT )
		pNode = new GradePresentEventUserNode;
	else if( eEventType == EVT_CONNECTION_TIME_SELECT_GIFT )
		pNode = new ConnectionTimeSelectGiftEventUserNode;
	else if( eEventType == EVT_ENTRY )
		pNode = new EntryEventUserNode;
	else if( eEventType == EVT_LADDER_POINT )
		pNode = new LadderPointEventUserNode;
	else if( eEventType == EVT_ANNOUNCE )
		pNode = new EventUserNode;
	else if( eEventType == EVT_ENTRY_AFTER )
		pNode = new EntryAfterEventUserNode;
	else if( eEventType == EVT_CONNECT_AND_PLAYTIME )
		pNode = new ConnectAndPlayTimeEventUserNode;
	else if( eEventType == EVT_ROULETTE )
		pNode = new EventUserNode;
	else if( eEventType == EVT_PLAZA_MONSTER )	// 광장 몬스터
		pNode = new PlazaMonsterEventUserNode;
	else if( eEventType == EVT_EXP2 )			// 경험치2
		pNode = new ExpEventUserNode;
	else if( eEventType == EVT_PES02 )			// 페소2
		pNode = new PesoEventUserNode;
	else if( eEventType == EVT_MODE_BONUS )		// 모드 보너스
		pNode = new ModeBonusEventUserNode;
	else if( eEventType == EVT_MODE_BONUS2 )	// 모드 보너스2
		pNode = new ModeBonusEventUserNode;
	else if( eEventType == EVT_MONSTER_DUNGEON)
		pNode = new MonsterDungeonEventUserNode;
	else if( eEventType == EVT_PCROOM_MONSTER_DUNGEON)
		pNode = new MonsterDungeonEventUserNode;
	else if( eEventType == EVT_FREEDAY_HERO )   // 태국 전용 프리데이 이벤트
		pNode = new FreeDayEventUserNode;
	else if( eEventType == EVT_HERO_EXP_BOOST )
		pNode = new HeroExpBoostEventUserNode;
	else if( eEventType == EVT_PCROOM_PLAYTIME )
		pNode = new PCRoomEventPlayTime;
	else
		EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Fail Create Event Node :%d", (int) eEventType );

	if( pNode )
	{
		pNode->SetType( eEventType );
		pNode->SetModeCategory( eModeCategory );
	}

	return pNode;
}

void EventUserManager::SetValue( EventType eEventType, int iArray, int iValue )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
		{
			(*it)->SetValue( iArray, iValue );
			break;
		}
	}
}

int EventUserManager::GetValue( EventType eEventType, int iArray )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
		{
			return (*it)->GetValue( iArray );
		}
	}

	return EVT_NONE;
}

EventUserNode * EventUserManager::GetEventUserNode( EventType eEventType, ModeCategory eModeCategory )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
		{
			if( (*it)->GetModeCategory() == eModeCategory )
				return (*it);
		}
	}

	return NULL;
}

void EventUserManager::SetIndex( EventType eEventType, DWORD dwIndex  )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
		{
			(*it)->SetIndex( dwIndex );
			break;
		}
	}	
}

void EventUserManager::Save( DWORD dwAgentID, const DWORD dwThreadID )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->Save( dwAgentID, dwThreadID );
	}	
}

void EventUserManager::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( (int)m_EventUserNodeVec.size() ) );

	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		PACKET_GUARD_VOID( rkPacket.Write( (*it)->GetType() ) );
		PACKET_GUARD_VOID( rkPacket.Write( (*it)->GetModeCategory() ) );
		PACKET_GUARD_VOID( rkPacket.Write( (*it)->GetFillMoveDataSize() ) );
		(*it)->FillMoveData( rkPacket, true );
	}	
}

void EventUserManager::ApplyMoveData( SP2Packet &rkPacket )
{
	// 각각의 노드의 사이즈가 가변적이므로 확인하여 처리함.
	int iNodeSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iNodeSize ) );

	int iVecSize = m_EventUserNodeVec.size();
	for (int i = 0; i < iNodeSize ; i++)
	{
		int iEventType        = 0;
		int iModeCategory	  = 0;
		int iFillMoveDataSize = 0;

		PACKET_GUARD_VOID( rkPacket.Read( iEventType ) );
		PACKET_GUARD_VOID( rkPacket.Read( iModeCategory ) );
		PACKET_GUARD_VOID( rkPacket.Read( iFillMoveDataSize ) );

		EventUserNode *pNode = NULL;
		if( COMPARE( i, 0, iVecSize ) )
			pNode = m_EventUserNodeVec[i];

		if( !pNode )
		{
			rkPacket.MovePointer( iFillMoveDataSize );
		}
		else if(    iEventType        != pNode->GetType() 
				 || iModeCategory	  != pNode->GetModeCategory()
			     || iFillMoveDataSize != pNode->GetFillMoveDataSize() )
		{
			rkPacket.MovePointer( iFillMoveDataSize );
		}
		else
		{
			pNode->ApplyMoveData(rkPacket);
		}
	}
}

void EventUserManager::Backup()
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->Backup();
	}	
}

void EventUserManager::Process( User *pUser )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->Process( pUser );
	}	
}

void EventUserManager::ProcessTime( User *pUser )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->ProcessTime( pUser );
	}	
}

int EventUserManager::GetNodeSize( EventType eEventType ) const
{
	for( EventUserNodeVec::const_iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
			return (*it)->GetSize();
	}

	return 0;
}

void EventUserManager::InsertDB( DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->InsertDB( dwAgentID, dwThreadID, szUserGUID, dwUserIndex );
	}	
}

void EventUserManager::SendData( User *pUser )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->SendData( pUser );
	}	
}

void EventUserManager::UpdateFirst( User *pUser )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		(*it)->UpdateFirst( pUser );
	}	
}

void EventUserManager::GetSameClassEventTypeVec( IN EventType eParentEventType, OUT IntVec &rvEventTypeVec )
{
	if( eParentEventType == EVT_BUY_ITEM )
	{
		rvEventTypeVec.push_back( EVT_BUY_ITEM );
		rvEventTypeVec.push_back( EVT_BUY_ITEM_2 );
		rvEventTypeVec.push_back( EVT_BUY_ITEM_3 );
	}
	else if( eParentEventType == EVT_ONE_DAY_GIFT )
	{
		rvEventTypeVec.push_back( EVT_ONE_DAY_GIFT );
		rvEventTypeVec.push_back( EVT_ONE_DAY_GIFT_2 );
	}
}