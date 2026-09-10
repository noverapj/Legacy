#include "stdafx.h"

#include "ioBlockEventAgency.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------

ioBlockEventListener::ioBlockEventListener()
{
	g_BlockEventAgency.RegisterEventListener( this );
}

ioBlockEventListener::~ioBlockEventListener()
{
	g_BlockEventAgency.UnRegisterEventListener( this );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

template<> ioBlockEventAgency* Singleton< ioBlockEventAgency >::ms_Singleton = 0;

ioBlockEventAgency::ioBlockEventAgency()
{
}

ioBlockEventAgency::~ioBlockEventAgency()
{
}

ioBlockEventAgency& ioBlockEventAgency::GetSingleton()
{
	return Singleton< ioBlockEventAgency >::GetSingleton();
}

void ioBlockEventAgency::RegisterEventListener( ioBlockEventListener* pListener )
{
	for( ioBlockEventListenerList::iterator iter = m_BlockEventListenerList.begin(); iter != m_BlockEventListenerList.end(); ++iter )
	{		
		if( pListener == *iter )
			return;
	}
	m_BlockEventListenerList.push_back( pListener );
}

void ioBlockEventAgency::UnRegisterEventListener( ioBlockEventListener* pListener )
{
	for( ioBlockEventListenerList::iterator iter = m_BlockEventListenerList.begin(); iter != m_BlockEventListenerList.end(); )
	{		
		if( pListener == *iter )
		{
			iter = m_BlockEventListenerList.erase( iter );
		}
		else
		{
			 ++iter;
		}
	}
}

bool ioBlockEventAgency::IsEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param )
{
	for( ReseveEventList::const_iterator iter = m_ReseveEventList.begin(); iter != m_ReseveEventList.end(); ++iter )
	{	
		const ReseveEvent& Event = *iter;
		if( Event.eType == eType && Event.pParam == param )
			return true;
	}

	return false;
}

void ioBlockEventAgency::SendEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param )
{
	if( !IsEvent( eType, param ) )
	{
		ReseveEvent Event;
		Event.eType		= eType;
		Event.pParam	= param;
		m_ReseveEventList.push_back( Event );
	}
}

void ioBlockEventAgency::ProcessEvent()
{
	for( ReseveEventList::const_iterator iter = m_ReseveEventList.begin(); iter != m_ReseveEventList.end(); ++iter )
	{	
		const ReseveEvent& Event = *iter;		
		BroadCastEvent( (ioBlockEventListener::EventType)Event.eType, Event.pParam );		
	}
	m_ReseveEventList.clear();
}

void ioBlockEventAgency::BroadCastEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param )
{
	for( ioBlockEventListenerList::iterator iter = m_BlockEventListenerList.begin(); iter != m_BlockEventListenerList.end(); ++iter )
	{
		ioBlockEventListener* pListener = *iter;
		if( pListener )
			pListener->ReciveEvent( eType, param );
	}
}