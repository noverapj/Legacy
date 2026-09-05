

#ifndef _ioAniEventHandler_h_
#define _ioAniEventHandler_h_

#include "io3DCommon.h"

struct AniEventTime;
class ioEntityGroup;
class ioAnimation;

struct AniEventInfo
{
	ioHashString m_AniName;
	ioHashString m_EventName;
};

class __EX ioAniEventHandler
{
protected:
	ioEntityGroup *m_pOwner;

	typedef std::vector< AniEventInfo > AniEventInfoList;

	AniEventInfoList m_HideMeshList;

public:
	void SetOwner( ioEntityGroup *pOwner );

public:
	virtual bool EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual bool ChangeAniEventHandling( const AniEventTime *pEvent, ioAnimation *pChangeAni );

	virtual void HideMeshExceptionCheck( const ioHashString &szMeshName );

	virtual void ClearAniEvent();
	virtual void ClearAniEffect();

protected:
	virtual void OnAutoSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnTrailStart( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnTrailEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnAnimationEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );

	virtual void OnShowMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnHideMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	ioAniEventHandler();
	virtual ~ioAniEventHandler();
};

#endif