

#ifndef _ioRcAnimation_h_
#define _ioRcAnimation_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioResource.h"
#include "ioRcAniTrack.h"

class ioAnimationEvent;
class ioRcAniTrack;

class __EX ioRcAnimation : public ioResource
{
protected:
	typedef std::vector< ioRcAniTrack > ioRcAniTrackVec;
	ioRcAniTrackVec	m_vAniTrackList;

	AniEventList m_EventTimeList;
	float	m_fFullTime;

	ioHashString m_szEventName;
	ioAnimationEvent* m_AnimationEvent;

public:
	void SetAnimationEvent( ioAnimationEvent* pEvent, const ioHashString& szEventName );
	const ioHashString& GetEventName(){ return m_szEventName; }

public:
	void SetFullTime( float fFullTime );
	void AllocAniTrackList( int iTrackCount );

public:
	void AddAniEvent( float fTime, const char *szEventType, const char *szSubInfo );
	void DeleteEvent( int iIdx );

	void ReserveAniEventCount( int iEventCount );
	void SortEventTimeList();

public:
	void ClearAniTrackList();
	void ClearAniEventList();

public:
	ioRcAniTrack* GetAniTrack( const ioHashString &szBipedName );
	ioRcAniTrack* GetAniTrack( int iTrackID );

	float GetTrackWeight( const ioHashString &kName );
	float GetTrackWeight( int iTrackID );

	inline float GetFullTime() const { return m_fFullTime; }
	inline int GetTrackCount() const { return m_vAniTrackList.size(); }

public:
	const AniEventList& GetEventList() const ;
	int GetEventCnt() const ;
	int GetEventCnt( const ioHashString &szEventType ) const;

	bool GetEvent( int iIdx,
		float &fEventTime,
		ioHashString &szEventType,
		ioHashString &szSubInfo );

	float GetEventTime( const ioHashString &szEventType ) const;

	int GetEventTimeList( const ioHashString &szEventType,
		std::list<float> &vTimeList ) const;

	int GetEventConstPtrList( const ioHashString &szEventType,
		AniEventConstPtrList &vEventList ) const;

public:
	ioRcAnimation( const ioHashString &name );
	virtual ~ioRcAnimation();
};

#endif
