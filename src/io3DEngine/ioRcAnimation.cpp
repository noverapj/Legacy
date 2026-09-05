

#include "StdAfx.h"

#include "ioRcAniTrack.h"
#include "ioRcAnimation.h"
#include "ioAnimationEvent.h"
#include "ioAnimationEventManager.h"

ioRcAnimation::ioRcAnimation( const ioHashString &name ) : ioResource( name )
{
	m_fFullTime = 0.0f;
	m_AnimationEvent = NULL;

	m_szEventName.Clear();
}

ioRcAnimation::~ioRcAnimation()
{
	ClearAniTrackList();
	ClearAniEventList();

	if( m_AnimationEvent )
	{
		ioAnimationEventManager::GetSingleton().Destroy( m_AnimationEvent );
		m_AnimationEvent = NULL;
	}
}

void ioRcAnimation::ClearAniTrackList()
{
	m_vAniTrackList.clear();
}

void ioRcAnimation::ClearAniEventList()
{
	m_EventTimeList.clear();
}

void ioRcAnimation::SetAnimationEvent( ioAnimationEvent* pEvent, const ioHashString& szEventName )
{
	m_AnimationEvent = pEvent;
	m_szEventName	 = szEventName;
}

void ioRcAnimation::SetFullTime( float fFullTime )
{
	m_fFullTime = fFullTime;
}

void ioRcAnimation::AllocAniTrackList( int iTrackCount )
{
	ClearAniTrackList();

	m_vAniTrackList.resize( iTrackCount );
}

void ioRcAnimation::AddAniEvent( float fTime, const char *szEventType, const char *szSubInfo )
{
	if( !m_AnimationEvent )
		m_EventTimeList.push_back( AniEventTime( fTime, szEventType, szSubInfo ) );	
}

void ioRcAnimation::DeleteEvent( int iIdx )
{
	if( m_AnimationEvent )
	{
		m_AnimationEvent->DeleteEvent( iIdx );
	}
	else
	{
		if( COMPARE( iIdx, 0, GetEventCnt() ) )
		{
			m_EventTimeList.erase( m_EventTimeList.begin() + iIdx );
		}
	}
}

void ioRcAnimation::ReserveAniEventCount( int iEventCount )
{
	if( m_AnimationEvent )
	{
		m_AnimationEvent->ReserveAniEventCount( iEventCount );
	}
	else
	{
		m_EventTimeList.reserve( iEventCount );
	}	
}

void ioRcAnimation::SortEventTimeList()
{
	if( m_AnimationEvent )
	{
		m_AnimationEvent->SortEventTimeList();
	}
	else
	{
		if( m_EventTimeList.size() > 1 )
		{
			std::sort( m_EventTimeList.begin(), m_EventTimeList.end() );
		}
	}
}

ioRcAniTrack* ioRcAnimation::GetAniTrack( const ioHashString &szBipedName )
{
	ioRcAniTrackVec::iterator iter;
	for( iter = m_vAniTrackList.begin() ; iter!=m_vAniTrackList.end(); ++iter )
	{
		if( iter->GetRcBipedName() == szBipedName )
			return &(*iter);
	}

	LOG.PrintTimeAndLog( 0, "ioRcAnimation::GetAniTrackByBipedID - %s Cannot Find (%s)", m_Name.c_str(), szBipedName.c_str() );

	return NULL;
}

ioRcAniTrack* ioRcAnimation::GetAniTrack( int id )
{
	if( COMPARE(id, 0, GetTrackCount() ) )
		return &m_vAniTrackList[id];

	LOG.PrintTimeAndLog( 0, "ioRcAnimation::GetAniTrack - %s over flow (%d/%d)", m_Name.c_str(), id, GetTrackCount() );
	return NULL;
}

float ioRcAnimation::GetTrackWeight( const ioHashString &kName )
{
	ioRcAniTrack *pTrack = GetAniTrack( kName );
	if( pTrack )
		return pTrack->GetWeight();

	return 0.0f;
}

float ioRcAnimation::GetTrackWeight( int iTrackID )
{
	ioRcAniTrack *pTrack = GetAniTrack( iTrackID );
	if( pTrack )
		return pTrack->GetWeight();

	return 0.0f;
}

int ioRcAnimation::GetEventCnt( const ioHashString &szEventType ) const
{
	int iEventCnt = 0;

	if( m_AnimationEvent )
	{
		iEventCnt = m_AnimationEvent->GetEventCnt( szEventType );
	}
	else
	{
		AniEventList::const_iterator iter;
		for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
		{
			if( iter->szEventType == szEventType )
				iEventCnt++;
		}
	}

	return iEventCnt;
}

float ioRcAnimation::GetEventTime( const ioHashString &szEventType ) const
{
	if( m_AnimationEvent )
	{
		return m_AnimationEvent->GetEventTime(  szEventType  );
	}
	else
	{
		AniEventList::const_iterator iter;
		for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
		{
			if( iter->szEventType == szEventType )
				return iter->fEventTime;
		}
	}
	return 0.0f;
}

const AniEventList& ioRcAnimation::GetEventList() const
{
	if( m_AnimationEvent )
	{		
		return m_AnimationEvent->GetEventList();
	}
	else
	{
		return m_EventTimeList;
	}

}
int ioRcAnimation::GetEventCnt() const
{
	if( m_AnimationEvent )
	{
		return m_AnimationEvent->GetEventCnt();
	}
	else
	{
		return m_EventTimeList.size();
	}	
}

bool ioRcAnimation::GetEvent( int iIdx, float &fEventTime, ioHashString &szEventType, ioHashString &szSubInfo )
{
	if( m_AnimationEvent )
	{		
		m_AnimationEvent->GetEvent( iIdx, fEventTime, szEventType, szSubInfo );
	}
	else
	{
		if( COMPARE( iIdx, 0, GetEventCnt() ) )
		{
			fEventTime  = m_EventTimeList[iIdx].fEventTime;
			szEventType = m_EventTimeList[iIdx].szEventType;
			szSubInfo   = m_EventTimeList[iIdx].szSubInfo;
			return true;
		}
	}

	LOG.PrintTimeAndLog( 0, "ioRcAnimation::GetEvent - Overflow(%d/%d)", iIdx, GetEventCnt() );

	return false;
}

int ioRcAnimation::GetEventTimeList( const ioHashString &szEventType, std::list<float> &vTimeList ) const
{
	if( m_AnimationEvent )
	{
		m_AnimationEvent->GetEventTimeList( szEventType, vTimeList );
	}
	else
	{
		AniEventList::const_iterator iter;
		for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
		{
			if( iter->szEventType == szEventType )
			{
				vTimeList.push_back( iter->fEventTime );
			}
		}
	}

	return vTimeList.size();
}

int ioRcAnimation::GetEventConstPtrList( const ioHashString &szEventType, AniEventConstPtrList &vEventList ) const
{
	if( m_AnimationEvent )
	{
		m_AnimationEvent->GetEventConstPtrList( szEventType, vEventList );
	}
	else
	{
		AniEventList::const_iterator iter;
		for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
		{
			if( iter->szEventType == szEventType )
			{
				vEventList.push_back( &(*iter) );
			}
		}
	}

	return vEventList.size();
}
