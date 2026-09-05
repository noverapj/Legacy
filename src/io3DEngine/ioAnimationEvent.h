#pragma once
#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioHashString.h"

#include "ioResource.h"

class ioAnimationPropertyManager;

class __EX ioAnimationEvent :  public ioResource
{
	friend class ioAnimationEventManager;
	typedef std::vector< AniEventTime > AniEventList;
	AniEventList m_EventTimeList;

	int		m_iVersion;
	float	m_fFullTime;

public:
	const char* GetVersionFormat() const;

public:
	void AddAniEvent( float fTime, const std::string &szEventType, const std::string &szSubInfo );
	void DeleteEvent( int iIdx );
	void DeleteAllEvent();

	void SortEventTimeList();
	void ReserveAniEventCount( int iEventCount );	

	bool IsEventExist( const char *szType, const char *szSubInfo );

	inline const AniEventList& GetEventList() const { return m_EventTimeList; }
	bool GetEvent( int iIdx, AniEventTime &rkEvent ) const;
	bool GetEvent( int iIdx, float &fEventTime, ioHashString &szEventType, ioHashString &szSubInfo );
	int GetEventTimeList( const ioHashString &szEventType, std::list<float> &vTimeList ) const;
	int GetEventConstPtrList( const ioHashString &szEventType, AniEventConstPtrList &vEventList ) const;

	float GetEventTime( const ioHashString &szEventType ) const;	

	int GetEventCnt() const;
	int GetEventCnt( const ioHashString &szEventType ) const;

public:
	const ioHashString& GetName();

public:
	ioAnimationEvent( const ioHashString &name );
	~ioAnimationEvent();
};
