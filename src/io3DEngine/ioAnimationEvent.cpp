

#include "stdafx.h"

#include "ioResourceManager.h"
#include "ioFileTokenDefine.h"
#include "ioINILoader.h"

#include "ioAnimationEvent.h"
#include "ioAnimationEventManager.h"

ioAnimationEvent::ioAnimationEvent( const ioHashString &name ) : ioResource( name )
{
	m_iVersion  = 0;
	m_fFullTime = 0.0f;
}

ioAnimationEvent::~ioAnimationEvent()
{
	m_EventTimeList.clear();
}

void ioAnimationEvent::AddAniEvent( float fTime, const std::string &szEventType, const std::string &szSubInfo )
{
	m_EventTimeList.push_back( AniEventTime( fTime, szEventType.c_str(), szSubInfo.c_str() ) );
	std::sort( m_EventTimeList.begin(), m_EventTimeList.end() );
}

const char* ioAnimationEvent::GetVersionFormat() const
{
	switch( m_iVersion )
	{
	case ANIMATION_VER_DEFAULT:
		return "UnCompress";
	case ANIMATION_VER_COMP4:
		return "4Bytes Compress";
	case ANIMATION_VER_COMP8:
		return "8Bytes Compress";
	}

	return "Unknown";
}

bool ioAnimationEvent::IsEventExist( const char *szType, const char *szSubInfo )
{
	int iEventCount = m_EventTimeList.size();
	for( int i=0 ; i<iEventCount ; i++ )
	{
		const AniEventTime &rkEvent = m_EventTimeList[i];

		if( rkEvent.szEventType == szType )
		{
			if( !strcmp( szSubInfo, "" ) )
				return true;

			if( strstr( rkEvent.szSubInfo.c_str(), szSubInfo ) )
				return true;
		}
	}

	return false;
}

bool ioAnimationEvent::GetEvent( int iIdx, AniEventTime &rkEvent ) const
{
	if( !COMPARE( iIdx, 0, GetEventCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAnimationEvent::GetEvent - Overflow(%d/%d)", iIdx, GetEventCnt() );
		return false;
	}

	rkEvent = m_EventTimeList[iIdx];

	return true;
}

bool ioAnimationEvent::GetEvent( int iIdx, float &fEventTime, ioHashString &szEventType, ioHashString &szSubInfo )
{
	if( COMPARE( iIdx, 0, GetEventCnt() ) )
	{
		fEventTime  = m_EventTimeList[iIdx].fEventTime;
		szEventType = m_EventTimeList[iIdx].szEventType;
		szSubInfo   = m_EventTimeList[iIdx].szSubInfo;
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioAnimationEvent::GetEvent - Overflow(%d/%d)", iIdx, GetEventCnt() );

	return false;
}

void ioAnimationEvent::DeleteEvent( int iIdx )
{
	if( COMPARE( iIdx, 0, GetEventCnt() ) )
	{ 
		m_EventTimeList.erase( m_EventTimeList.begin() + iIdx );
	}
}

void ioAnimationEvent::DeleteAllEvent()
{
	m_EventTimeList.clear();
}

int ioAnimationEvent::GetEventCnt() const
{
	return m_EventTimeList.size();
}

int ioAnimationEvent::GetEventCnt( const ioHashString &szEventType ) const
{
	int iEventCnt = 0;

	AniEventList::const_iterator iter;
	for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
	{
		if( iter->szEventType == szEventType )
			iEventCnt++;
	}

	return iEventCnt;
}

float ioAnimationEvent::GetEventTime( const ioHashString &szEventType ) const
{
	AniEventList::const_iterator iter;
	for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
	{
		if( iter->szEventType == szEventType )
			return iter->fEventTime;
	}

	return 0.0f;
}

int ioAnimationEvent::GetEventTimeList( const ioHashString &szEventType, std::list<float> &vTimeList ) const
{
	AniEventList::const_iterator iter;
	for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
	{
		if( iter->szEventType == szEventType )
		{
			vTimeList.push_back( iter->fEventTime );
		}
	}

	return vTimeList.size();
}

int ioAnimationEvent::GetEventConstPtrList( const ioHashString &szEventType, AniEventConstPtrList &vEventList ) const
{
	AniEventList::const_iterator iter;
	for( iter=m_EventTimeList.begin() ; iter!=m_EventTimeList.end() ; ++iter )
	{
		if( iter->szEventType == szEventType )
		{
			vEventList.push_back( &(*iter) );
		}
	}

	return vEventList.size();
}


void ioAnimationEvent::SortEventTimeList()
{
	if( m_EventTimeList.size() > 1 )
	{
		std::sort( m_EventTimeList.begin(), m_EventTimeList.end() );
	}
}

void ioAnimationEvent::ReserveAniEventCount( int iEventCount )
{
	m_EventTimeList.reserve( iEventCount );	
}

const ioHashString& ioAnimationEvent::GetName()
{
	return m_Name;
}