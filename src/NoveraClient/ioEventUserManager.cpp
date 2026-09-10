#include "stdafx.h"

#include "ioEventUserManager.h"

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
		EventUserNode *pNode = CreatEventUserNode( g_EventMgr.GetType( i ) );
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

EventUserNode * EventUserManager::CreatEventUserNode( EventType eEventType )
{
	EventUserNode *pNode = NULL;

	switch( eEventType )
	{
	case EVT_NONE:
		pNode = new EventUserNode;
		break;
	case EVT_PROPOSAL:
		pNode = new ProposalEventUserNode;
		break;
	case EVT_COIN:
		pNode = new CoinEventUserNode;
		break;
	case EVT_EXP:
		pNode = new ExpEventUserNode;
		break;
	case EVT_PESO:
		pNode = new PesoEventUserNode;
		break;
	case EVT_PLAYTIME:
		pNode = new PlayTimeEventUserNode;
		break;
	case EVT_CHILDRENDAY:
		pNode = new ChildrenDayEventUserNode;
		break;
	case EVT_PESOBONUS:
		pNode = new PesoBonusEventUserNode;
		break;
	case EVT_BUY_CHAR_NO_LEVEL_LIMIT:
		pNode = new BuyCharNoLevelLimitEventUserNode;
		break;
	case EVT_GRADEUP:
		pNode = new GradeUpEventUserNode;
		break;
	case EVT_PCROOM_BONUS:
		pNode = new PCRoomEventUserNode;
		break;
	case EVT_CHANCE_MORTMAIN_CHAR:
		pNode = new ChanceMortmainCharEventUserNode;
		break;
	case EVT_ONE_DAY_GOLD_ITEM:
		pNode = new OneDayGoldItemEvent;
		break;
	case EVT_DORMANCY_USER:
		pNode = new DormancyUserEvent;
		break;
	case EVT_PLAYTIME_PRESENT:
		pNode = new PlayTimePresentEventUserNode;
		break;
	case EVT_CHRISTMAS:
		pNode = new ChristmasEventUserNode;
		break;
	case EVT_BUY_ITEM:
	case EVT_BUY_ITEM_2:
	case EVT_BUY_ITEM_3:
		pNode = new BuyItemEventUserNode;
		break;
	case EVT_FISHING:
		pNode = new FishingEventUserNode;
		break;
	case EVT_EXERCISESOLDIER:
		pNode = new ExerciseSoldierEventUserNode;
		break;
	case EVT_CONNECTION_TIME:
		pNode = new ConnectionTimeEventUserNode;
		break;
	case EVT_ONE_DAY_GIFT:
	case EVT_ONE_DAY_GIFT_2:
		pNode = new OneDayGiftEventUserNode;
		break;
	case EVT_GRADEUP_PRESENT:
		pNode = new GradePresentEventUserNode;
		break;
	case EVT_CONNECTION_TIME_SELECT_GIFT:
		pNode = new ConnectionTimeSelectGiftEventUserNode;
		break;
	case EVT_ENTRY:
		pNode = new EntryEventUserNode;
		break;
	case EVT_LADDER_POINT:
		pNode = new LadderPointEventUserNode;
		break;
	case EVT_ANNOUNCE:
	case EVT_ENTRY_AFTER:
		pNode = new EventUserNode;
		break;
	case EVT_CONNECT_AND_PLAYTIME:
		pNode = new ConnectAndPlayTimeEventUserNode;
		break;
	case EVT_PLAZA_MONSTER:
	case EVT_EXP2:
	case EVT_PES02:
	case EVT_MODE_BONUS:
	case EVT_MODE_BONUS2:
		break;
	case EVT_FREEDAY_HERO:
		pNode = new FreeDayEventUserNode;
		break;
	default:
		LOG.PrintTimeAndLog( 0, "Fail Create Event Node :%d", (int) eEventType );
		break;
	}

	if( pNode )
		pNode->SetType( eEventType );

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

EventUserNode * EventUserManager::GetEventUserNode( EventType eEventType )
{
	for(EventUserNodeVec::iterator it = m_EventUserNodeVec.begin(); it != m_EventUserNodeVec.end(); ++it)
	{
		if( (*it)->GetType() == eEventType )
		{
			return (*it);
		}
	}

	return NULL;
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